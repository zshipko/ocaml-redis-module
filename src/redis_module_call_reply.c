#include "ocaml_redis_module.h"

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
