#include "ocaml_redis_module.h"

value rstring_to_string(value str) {
  CAMLparam1(str);
  CAMLlocal1(r);
  size_t len;
  const char *s = RedisModule_StringPtrLen(Rstring_val(str), &len);
  r = caml_alloc_string(len);
  memcpy(String_val(r), s, len);
  CAMLreturn(r);
}

value rstring_to_int64(value str) {
  CAMLparam1(str);
  CAMLlocal1(r);
  long long n = 0;
  RedisModule_StringToLongLong(Rstring_val(str), &n);
  r = caml_copy_int64(n);
  CAMLreturn(r);
}

value rstring_to_float(value str) {
  CAMLparam1(str);
  CAMLlocal1(r);
  double d = 0.0;
  RedisModule_StringToDouble(Rstring_val(str), &d);
  r = caml_copy_double(d);
  CAMLreturn(r);
}

value rstring_append(value ctx, value str, value b) {
  CAMLparam3(ctx, str, b);
  CAMLlocal1(r);
  r = Val_int(RedisModule_StringAppendBuffer(Context_val(ctx), Rstring_val(str),
                                             String_val(b),
                                             caml_string_length(b)));
  CAMLreturn(r);
}

value rstring_compare(value a, value b) {
  CAMLparam2(a, b);
  CAMLlocal1(r);
  r = Val_int(RedisModule_StringCompare(Rstring_val(a), Rstring_val(b)));
  CAMLreturn(r);
}

value rstring_from_call_reply(value cr) {
  CAMLparam1(cr);
  CAMLlocal1(r);
  r = Val_rstring(RedisModule_CreateStringFromCallReply(Call_reply_val(cr)));
  CAMLreturn(r);
}

value rstring_from_string(value ctx, value str) {
  CAMLparam2(ctx, str);
  CAMLlocal1(r);
  r = Val_rstring(RedisModule_CreateString(Context_val(ctx), String_val(str),
                                           caml_string_length(str)));
  CAMLreturn(r);
}

value rstring_copy(value ctx, value rs) {
  CAMLparam2(ctx, rs);
  CAMLlocal1(r);
  r = (value)RedisModule_CreateStringFromString((RedisModuleCtx *)ctx,
                                                (RedisModuleString *)rs);
  CAMLreturn(r);
}
