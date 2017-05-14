#include "ocaml_redis_module.h"

const value OK = Val_int(0);
const value ERR = Val_int(1);

value Some(value x) {
    value dst = caml_alloc_small(1, 0);
    Store_field(dst, 0, x);
    return dst;
}

const value None = Val_int(0);

// Arguments to initialize OCaml instance with
const char *default_args[] = {"redis-server", NULL};

int internalNamedCallback(const char *name, RedisModuleCtx *ctx, RedisModuleString **argv, int argc){

    value *fn = caml_named_value(name);
    if (!fn){
        return REDISMODULE_ERR;
    }

    int i;
    size_t len = 0;
    value args = argc == 0 ? Atom(0) : caml_alloc(argc, 0), x;
    for(i = 0; i < argc; i++){
        Store_field(args, i, (value)argv[i]);
    }

    return Int_val(caml_callback2(*fn, (value)ctx, args));
}

// Wrap OCaml callbacks
int internalCommandWrapper(RedisModuleCtx *ctx, RedisModuleString **argv, int argc){
    RedisModule_AutoMemory(ctx);
    size_t cmdSize = 0;
    const char *cmd = RedisModule_StringPtrLen(argv[0], &cmdSize);
    return internalNamedCallback(cmd, ctx, argv, argc);
}

// Define generic RedisModule_OnLoad
int RedisModule_OnLoad(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    caml_startup((char**)default_args);
    return internalNamedCallback("redis_module_on_load", ctx, argv, argc);
}

value module_init (value _ctx, value name, value ver, value api_ver)
{
    CAMLparam4(_ctx, name, ver, api_ver);
    RedisModuleCtx *ctx = (RedisModuleCtx*)_ctx;
    CAMLlocal1(r);
    r = Val_int(RedisModule_Init(ctx, String_val(name), Int_val(ver), Int_val(api_ver)));
}

value module_create_command_internal(value _ctx, value name, value flags, value keyinfo){
    CAMLparam4(_ctx, name, flags, keyinfo);
    CAMLreturn(RedisModule_CreateCommand((RedisModuleCtx*)_ctx, String_val(name), internalCommandWrapper, String_val(flags), Int_val(Field(keyinfo, 0)), Int_val((Field(keyinfo, 1))), Int_val(Field(keyinfo, 2))));
}

value module_get_selected_db(value ctx){
    CAMLparam1(ctx);
    CAMLreturn(Val_int(RedisModule_GetSelectedDb((RedisModuleCtx*)ctx)));
}

value module_select_db(value ctx, value db){
    CAMLparam1(ctx);
    CAMLreturn(Val_int(RedisModule_SelectDb((RedisModuleCtx*)ctx, Int_val(db))));
}

value module_replicate_internal(value ctx, value cmd, value args){
    CAMLparam3(ctx, cmd, args);
    CAMLreturn(RedisModule_Replicate((RedisModuleCtx*)ctx, String_val(cmd), "b", String_val(args), caml_string_length(args)));
}

value module_replicate_verbatim(value ctx){
    CAMLparam1(ctx);
    CAMLreturn(Val_int(RedisModule_ReplicateVerbatim((RedisModuleCtx*)ctx)));
}

// Call reply

void call_reply_free(value cr){
    CAMLparam1(cr);
    RedisModule_FreeCallReply(Data_custom_val(cr));
}

struct custom_operations call_reply_ops = {
    "redis_module.call_reply",
    call_reply_free,
    custom_compare_default,
    custom_hash_default,
    custom_serialize_default,
    custom_deserialize_default,
    custom_compare_ext_default
};

struct custom_operations call_reply_nofree_ops = {
    "redis_module.call_reply_nofree",
    custom_finalize_default,
    custom_compare_default,
    custom_hash_default,
    custom_serialize_default,
    custom_deserialize_default,
    custom_compare_ext_default
};

#define Call_reply_val(v) (*(RedisModuleCallReply**)Data_custom_val(v))

value module_call_internal(value ctx, value cmd, value args){
    CAMLparam3(ctx, cmd, args);
    RedisModuleCallReply *reply = RedisModule_Call((RedisModuleCtx*)ctx, String_val(cmd), "b", String_val(args), caml_string_length(args));
    if (!reply){
        caml_failwith("Invalid reply");
        CAMLreturn(Val_unit);
    }
    value r = caml_alloc_custom(&call_reply_ops, sizeof(RedisModuleCallReply*), 0, 1);
    Call_reply_val(r) = reply;
    CAMLreturn(r);
}

// Call reply

value call_reply_proto(value cr){
    CAMLparam1(cr);
    size_t len;
    const char *s = RedisModule_CallReplyProto(Call_reply_val(cr), &len);
    CAMLlocal1(dst);
    dst = caml_alloc_string(len);
    memcpy(String_val(dst), s, len);
    CAMLreturn(dst);
}

value call_reply_to_string(value cr){
    CAMLparam1(cr);
    size_t len;
    const char *s = RedisModule_CallReplyStringPtr(Call_reply_val(cr), &len);
    if (!s){
        CAMLreturn(None);
    }
    CAMLlocal1(dst);
    dst = caml_alloc_string(len);
    memcpy(String_val(dst), s, len);
    CAMLreturn(Some(dst));
}

value call_reply_to_int64(value cr){
    CAMLparam1(cr);

    if (RedisModule_CallReplyType(Call_reply_val(cr)) != REDISMODULE_REPLY_INTEGER){
        return None;
    }

    long long n = RedisModule_CallReplyInteger(Call_reply_val(cr));
    CAMLreturn(Some(caml_copy_int64(n)));
}

value call_reply_get_type(value cr){
    CAMLparam1(cr);
    CAMLreturn(Int_val(RedisModule_CallReplyType(Call_reply_val(cr))));
}

value call_reply_length(value cr){
    CAMLparam1(cr);
    CAMLreturn(caml_copy_int64(RedisModule_CallReplyLength(Call_reply_val(cr))));
}

value call_reply_index(value cr, value i){
    CAMLparam2(cr, i);
    RedisModuleCallReply *reply = RedisModule_CallReplyArrayElement(Call_reply_val(cr), Int64_val(i));
    if (reply == NULL){
        CAMLreturn(None);
    }

    CAMLlocal1(r);
    r = caml_alloc_custom(&call_reply_nofree_ops, sizeof(RedisModuleCallReply*), 0, 1);
    Call_reply_val(r) = reply;
    CAMLreturn(Some(r));
}

// Reply
value reply_wrong_arity(value _ctx){
    return Val_int(RedisModule_WrongArity((RedisModuleCtx*)_ctx));
}

value reply_int64(value _ctx, value i){
    return Val_int(RedisModule_ReplyWithLongLong((RedisModuleCtx*)_ctx, Int64_val(i)));
}

value reply_int(value _ctx, value i){
    return Val_int(RedisModule_ReplyWithLongLong((RedisModuleCtx*)_ctx, Int_val(i)));
}

value reply_simple_string(value _ctx, value s){
    return Val_int(RedisModule_ReplyWithSimpleString((RedisModuleCtx*)_ctx, String_val(s)));
}

value reply_error(value _ctx, value s){
    return Val_int(RedisModule_ReplyWithError((RedisModuleCtx*)_ctx, String_val(s)));
}

value reply_null(value _ctx){
    return Val_int(RedisModule_ReplyWithNull((RedisModuleCtx*)_ctx));
}

value reply_array(value _ctx, value i){
    return Val_int(RedisModule_ReplyWithArray((RedisModuleCtx*)_ctx, Int_val(i)));
}

value reply_set_array_length(value _ctx, value i){
    RedisModule_ReplySetArrayLength((RedisModuleCtx*)_ctx, Int_val(i));
    return Val_unit;
}

value reply_string(value _ctx, value s){
    return Val_int(RedisModule_ReplyWithStringBuffer((RedisModuleCtx*)_ctx, String_val(s), caml_string_length(s)));
}

value reply_rstring(value _ctx, value s){
    return Val_int(RedisModule_ReplyWithString((RedisModuleCtx*)_ctx, (RedisModuleString*)s));
}

value reply_float(value _ctx, value f){
    return Val_int(RedisModule_ReplyWithDouble((RedisModuleCtx*)_ctx, Double_val(f)));
}

value reply_call_reply(value _ctx, value reply){
    return Val_int(RedisModule_ReplyWithCallReply((RedisModuleCtx*)_ctx, (RedisModuleCallReply*)reply));
}

// Rstring



