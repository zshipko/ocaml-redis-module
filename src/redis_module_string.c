#include "ocaml_redis_module.h"

value rstring_to_string(value str){
    CAMLparam1(str);
    size_t len;
    const char *s = RedisModule_StringPtrLen((RedisModuleString*)str, &len);
    CAMLlocal1(dst);
    dst = caml_alloc_string(len);
    memcpy(String_val(dst), s, len);
    CAMLreturn(dst);
}

value rstring_to_int64(value str){
    CAMLparam1(str);
    long long n = 0;
    RedisModule_StringToLongLong((RedisModuleString*)str, &n);
    CAMLreturn(caml_copy_int64(n));
}

value rstring_to_float(value str){
    CAMLparam1(str);
    double d = 0.0;
    RedisModule_StringToDouble((RedisModuleString*)str, &d);
    CAMLreturn(caml_copy_double(d));
}

value rstring_append(value ctx, value str, value b){
    return(Val_int(RedisModule_StringAppendBuffer((RedisModuleCtx*)ctx, (RedisModuleString*)str, String_val(b), caml_string_length(b))));
}

value rstring_compare(value a, value b){
    return(Val_int(RedisModule_StringCompare((RedisModuleString*)a, (RedisModuleString*)b)));
}

value rstring_from_call_reply(value cr){
    return((value)RedisModule_CreateStringFromCallReply((RedisModuleCallReply*)cr));
}

value rstring_from_string(value ctx, value str){
    CAMLparam2(ctx, str);
    CAMLreturn((value)RedisModule_CreateString((RedisModuleCtx*)ctx, String_val(str), caml_string_length(str)));
}

value rstring_copy(value ctx, value rs){
    CAMLparam2(ctx, rs);
    CAMLreturn((value)RedisModule_CreateStringFromString((RedisModuleCtx*)ctx, (RedisModuleString*)rs));
}
