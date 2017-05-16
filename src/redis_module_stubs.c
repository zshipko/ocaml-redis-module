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

value args_get(value a, value i){
    Args *args = Args_val(a);

    if (args->argc > Int_val(i)){
        return Val_rstring(args->arg[Int_val(i)]);
    }

    caml_failwith("Invalid argument");
    return Val_unit;
}

value args_length(value a){
    Args *args = Args_val(a);
    return Val_int(args->argc);
}

int internalNamedCallback(const char *name, RedisModuleCtx *ctx, value args){
    value *fn = caml_named_value(name);
    if (!fn){
        return REDISMODULE_ERR;
    }

    return Int_val(caml_callback2(*fn, Val_context(ctx), args));
}

// Wrap OCaml callbacks
int internalCommandWrapper(RedisModuleCtx *ctx, RedisModuleString **argv, int argc){
    RedisModule_AutoMemory(ctx);

    size_t cmdSize = 0;
    const char *cmd = RedisModule_StringPtrLen(argv[0], &cmdSize);
    if (!cmd) {
        return REDISMODULE_ERR;
    }

    Args args = {argc, argv};
    return internalNamedCallback(cmd, ctx, Val_args(&args));
}

// Define generic RedisModule_OnLoad
int RedisModule_OnLoad(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    caml_startup((char**)default_args);

    Args args = {argc, argv};
    return internalNamedCallback("redis_module_on_load", ctx, Val_args(&args));
}

value module_init (value _ctx, value name, value ver, value api_ver)
{
    RedisModuleCtx *ctx = (RedisModuleCtx*)_ctx;
    return Val_int(RedisModule_Init(ctx, String_val(name), Int_val(ver), Int_val(api_ver)));
}

value module_create_command_internal(value _ctx, value name, value flags, value keyinfo){
    const char *cmd = String_val(name);
    value *fn = caml_named_value(cmd);
    if (!fn){
        return (value)REDISMODULE_ERR;
    }

    return((value)RedisModule_CreateCommand(Context_val(_ctx), cmd, internalCommandWrapper, String_val(flags), Int_val(Field(keyinfo, 0)), Int_val((Field(keyinfo, 1))), Int_val(Field(keyinfo, 2))));
}

value module_get_selected_db(value ctx){
    return(Val_int(RedisModule_GetSelectedDb(Context_val(ctx))));
}

value module_select_db(value ctx, value db){
    return(Val_int(RedisModule_SelectDb(Context_val(ctx), Int_val(db))));
}

value module_replicate_internal(value ctx, value cmd, value args){
    return(RedisModule_Replicate(Context_val(ctx), String_val(cmd), "b", String_val(args), caml_string_length(args)));
}

value module_replicate_verbatim(value ctx){
    return(Val_int(RedisModule_ReplicateVerbatim(Context_val(ctx))));
}


// Reply
value reply_wrong_arity(value ctx){
    return(Val_int(RedisModule_WrongArity(Context_val(ctx))));
}

value reply_int64(value _ctx, value i){
    return(Val_int(RedisModule_ReplyWithLongLong(Context_val(_ctx), Int64_val(i))));
}

value reply_int(value _ctx, value i){
    CAMLparam2(_ctx, i);
    CAMLreturn(Val_int(RedisModule_ReplyWithLongLong(Context_val(_ctx), Int_val(i))));
}

value reply_simple_string(value _ctx, value s){
    return(Val_int(RedisModule_ReplyWithSimpleString(Context_val(_ctx), String_val(s))));
}

value reply_error(value _ctx, value s){
    return(Val_int(RedisModule_ReplyWithError(Context_val(_ctx), String_val(s))));
}

value reply_null(value _ctx){
    return(Val_int(RedisModule_ReplyWithNull(Context_val(_ctx))));
}

value reply_array(value _ctx, value i){
    return (Val_int(RedisModule_ReplyWithArray(Context_val(_ctx), Int_val(i))));
}

value reply_set_array_length(value _ctx, value i){
    RedisModule_ReplySetArrayLength(Context_val(_ctx), Int_val(i));
    return(Val_unit);
}

value reply_string(value _ctx, value s){
    return(Val_int(RedisModule_ReplyWithStringBuffer(Context_val(_ctx), String_val(s), caml_string_length(s))));
}

value reply_rstring(value _ctx, value s){
    return(Val_int(RedisModule_ReplyWithString(Context_val(_ctx), Rstring_val(s))));
}

value reply_float(value _ctx, value f){
    return (Val_int(RedisModule_ReplyWithDouble(Context_val(_ctx), Double_val(f))));
}

value reply_call_reply(value _ctx, value reply){
    return (Val_int(RedisModule_ReplyWithCallReply(Context_val(_ctx), Call_reply_val(reply))));
}

