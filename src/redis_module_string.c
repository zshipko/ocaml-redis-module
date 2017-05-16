#include "ocaml_redis_module.h"

value rstring_to_string(value str){
    CAMLparam1(str);
    size_t len;
    const char *s = RedisModule_StringPtrLen(Rstring_val(str), &len);
    CAMLlocal1(dst);
    dst = caml_alloc_string(len);
    memcpy(String_val(dst), s, len);
    CAMLreturn(dst);
}

value rstring_to_int64(value str){
    CAMLparam1(str);
    long long n = 0;
    RedisModule_StringToLongLong(Rstring_val(str), &n);
    CAMLreturn(caml_copy_int64(n));
}

value rstring_to_float(value str){
    CAMLparam1(str);
    double d = 0.0;
    RedisModule_StringToDouble(Rstring_val(str), &d);
    CAMLreturn(caml_copy_double(d));
}

value rstring_append(value ctx, value str, value b){
    return (Val_int(RedisModule_StringAppendBuffer(Context_val(ctx), Rstring_val(str), String_val(b), caml_string_length(b))));
}

value rstring_compare(value a, value b){
    return Val_int(RedisModule_StringCompare(Rstring_val(a), Rstring_val(b)));
}

value rstring_from_call_reply(value cr){
    return Val_rstring(RedisModule_CreateStringFromCallReply(Call_reply_val(cr)));
}

value rstring_from_string(value ctx, value str){
    CAMLparam2(ctx, str);
    CAMLreturn (Val_rstring(RedisModule_CreateString(Context_val(ctx), String_val(str), caml_string_length(str))));
}

value rstring_copy(value ctx, value rs){
    CAMLparam2(ctx, rs);
    CAMLreturn((value)RedisModule_CreateStringFromString((RedisModuleCtx*)ctx, (RedisModuleString*)rs));
}
