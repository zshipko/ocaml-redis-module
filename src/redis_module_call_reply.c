#include "ocaml_redis_module.h"

void call_reply_free(value cr){
    CAMLparam1(cr);
    RedisModule_FreeCallReply(Data_custom_val(cr));
}

#define Call_reply_val(v) ((RedisModuleCallReply*)v)

value module_call_internal(value ctx, value cmd, value args){
    RedisModuleCallReply *reply = RedisModule_Call((RedisModuleCtx*)ctx, String_val(cmd), "b", String_val(args), caml_string_length(args));
    if (!reply){
        caml_failwith("Invalid reply");
        return Val_unit;
    }

    return (value)reply;
}

// Call reply

value call_reply_proto(value cr){
    size_t len;
    const char *s = RedisModule_CallReplyProto(Call_reply_val(cr), &len);
    value dst = caml_alloc_string(len);
    memcpy(String_val(dst), s, len);
    return dst;
}

value call_reply_to_string(value cr){
    size_t len;
    const char *s = RedisModule_CallReplyStringPtr(Call_reply_val(cr), &len);
    if (!s){
        return None;
    }
    value dst = caml_alloc_string(len);
    memcpy(String_val(dst), s, len);
    return(Some(dst));
}

value call_reply_to_int64(value cr){
    if (RedisModule_CallReplyType(Call_reply_val(cr)) != REDISMODULE_REPLY_INTEGER){
        return None;
    }

    long long n = RedisModule_CallReplyInteger(Call_reply_val(cr));
    return Some(caml_copy_int64(n));
}

value call_reply_get_type(value cr){
    return(Int_val(RedisModule_CallReplyType(Call_reply_val(cr))));
}

value call_reply_length(value cr){
    return (caml_copy_int64(RedisModule_CallReplyLength(Call_reply_val(cr))));
}

value call_reply_index(value cr, value i){
    RedisModuleCallReply *reply = RedisModule_CallReplyArrayElement(Call_reply_val(cr), Int64_val(i));
    if (reply == NULL){
        return None;
    }
    return Some((value)reply);
}
