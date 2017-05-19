#include "ocaml_redis_module.h"

value value_to_string(value str){
    CAMLparam1(str);
    size_t len;
    const char *s = RedisModule_StringPtrLen(Value_val(str), &len);
    CAMLlocal1(dst);
    dst = caml_alloc_string(len);
    memcpy(String_val(dst), s, len);
    CAMLreturn(dst);
}

value value_to_int64(value str){
    CAMLparam1(str);
    long long n = 0;
    RedisModule_StringToLongLong(Value_val(str), &n);
    CAMLreturn(caml_copy_int64(n));
}

value value_to_float(value str){
    CAMLparam1(str);
    double d = 0.0;
    RedisModule_StringToDouble(Value_val(str), &d);
    CAMLreturn(caml_copy_double(d));
}

value value_append(value ctx, value str, value b){
    return (Val_int(RedisModule_StringAppendBuffer(Context_val(ctx), Value_val(str), String_val(b), caml_string_length(b))));
}

value value_compare(value a, value b){
    return Val_int(RedisModule_StringCompare(Value_val(a), Value_val(b)));
}

value value_from_call_reply(value cr){
    return Val_value(RedisModule_CreateStringFromCallReply(Call_reply_val(cr)));
}

value value_from_string(value ctx, value str){
    CAMLparam2(ctx, str);
    CAMLreturn (Val_value(RedisModule_CreateString(Context_val(ctx), String_val(str), caml_string_length(str))));
}

value value_copy(value ctx, value rs){
    CAMLparam2(ctx, rs);
    CAMLreturn((value)RedisModule_CreateStringFromString((RedisModuleCtx*)ctx, (RedisModuleString*)rs));
}
