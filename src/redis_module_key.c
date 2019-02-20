#include "ocaml_redis_module.h"

#define convert_mode(_mode)                                                    \
  Int_val(_mode) == 0                                                          \
      ? REDISMODULE_READ                                                       \
      : Int_val(_mode) == 1 ? REDISMODULE_WRITE                                \
                            : REDISMODULE_READ | REDISMODULE_WRITE
#define convert_hash_flag(_hf)                                                 \
  Int_val(_hf) == 1                                                            \
      ? REDISMODULE_HASH_NX                                                    \
      : Int_val(_hf) == 2 ? REDISMODULE_HASH_XX                                \
                          : Int_val(_hf) == 3 ? REDISMODULE_HASH_EXISTS        \
                                              : REDISMODULE_HASH_NONE

value key_find(value ctx, value keyname, value mode) {
  CAMLparam3(ctx, keyname, mode);
  CAMLlocal1(r);
  r = Val_key(RedisModule_OpenKey(Context_val(ctx), Rstring_val(keyname),
                                  convert_mode(mode)));
  CAMLreturn(r);
}

value key_type(value key) {
  CAMLparam1(key);
  CAMLlocal1(r);
  r = Val_int(RedisModule_KeyType(Key_val(key)));
  CAMLreturn(r);
}

value key_length(value key) {
  CAMLparam1(key);
  CAMLlocal1(r);
  r = caml_copy_int64(RedisModule_ValueLength(Key_val(key)));
  CAMLreturn(r);
}

value key_list_push(value key, value where, value v) {
  CAMLparam3(key, where, v);
  CAMLlocal1(r);
  r = Val_int(
      RedisModule_ListPush(Key_val(key), Int_val(where), Rstring_val(v)));
  CAMLreturn(r);
}

value key_list_pop(value key, value where) {
  CAMLparam2(key, where);
  CAMLlocal1(r);
  RedisModuleString *str = RedisModule_ListPop(Key_val(key), Int_val(where));
  if (!str) {
    r = None;
  } else {
    r = Some(Val_rstring(str));
  }

  CAMLreturn(r);
}

value key_delete(value key) {
  CAMLparam1(key);
  CAMLlocal1(r);
  r = Val_int(RedisModule_DeleteKey(Key_val(key)));
  CAMLreturn(r);
}

value key_set(value key, value v) {
  CAMLparam2(key, v);
  CAMLlocal1(r);
  r = Val_int(RedisModule_StringSet(Key_val(key), Rstring_val(v)));
  CAMLreturn(r);
}

value key_get_string(value key) {
  CAMLparam1(key);
  size_t len;
  const char *c = RedisModule_StringDMA(Key_val(key), &len, REDISMODULE_READ);
  if (!c) {
    CAMLreturn(None);
  }

  CAMLlocal1(v);
  v = caml_alloc_string(len);
  memcpy(String_val(v), c, len);
  CAMLreturn(Some(v));
}

value key_truncate(value key, value size) {
  CAMLparam2(key, size);
  CAMLlocal1(r);
  r = Val_int(RedisModule_StringTruncate(Key_val(key), Int64_val(size)));
  CAMLreturn(r);
}

value key_get_expire(value key) {
  CAMLparam1(key);
  CAMLlocal1(r);
  r = caml_copy_int64(RedisModule_GetExpire(Key_val(key)));
  CAMLreturn(r);
}

value key_set_expire(value key, value i) {
  CAMLparam2(key, i);
  CAMLlocal1(r);
  r = Val_int(RedisModule_SetExpire(Key_val(key), Int64_val(i)));
  CAMLreturn(r);
}

value key_hash_delete(value unit) {
  CAMLparam1(unit);
  CAMLlocal1(r);
  r = Val_rstring(REDISMODULE_HASH_DELETE);
  CAMLreturn(r);
}

value key_hash_set(value key, value flag, value arr) {
  CAMLparam3(key, flag, arr);
  CAMLlocal1(r);
  r = OK;

  int len = caml_array_length(arr);
  int i;

  for (i = 0; i < len; i++) {
    value v = Field(arr, i);
    if (RedisModule_HashSet(Key_val(key), convert_hash_flag(flag),
                            Rstring_val(Field(v, 0)), Rstring_val(Field(v, 1)),
                            NULL) != REDISMODULE_OK) {
      r = ERR;
      break;
    }
  }

  CAMLreturn(r);
}

value key_hash_get(value key, value flag, value arr) {
  CAMLparam3(key, flag, arr);
  int len = caml_array_length(arr);
  int i;

  CAMLlocal2(dst, r);
  dst = caml_alloc(len, 0);

  RedisModuleString *s;
  for (i = 0; i < len; i++) {
    value v = Field(arr, i);
    if (RedisModule_HashGet(Key_val(key), convert_hash_flag(flag),
                            Rstring_val(v), &s, NULL) != REDISMODULE_OK) {
      CAMLreturn(None);
    }
    Field(dst, i) = Val_rstring(s);
  }

  r = Some(dst);
  CAMLreturn(r);
}

value key_zset_add(value key, value score, value elem) {
  CAMLparam3(key, score, elem);
  CAMLlocal1(r);
  r = Val_int(RedisModule_ZsetAdd(Key_val(key), Double_val(score),
                                  Rstring_val(elem), NULL));
  CAMLreturn(r);
}

value key_zset_score(value key, value elem) {
  CAMLparam2(key, elem);
  CAMLlocal2(dst, r);

  double d;
  if (RedisModule_ZsetScore(Key_val(key), Rstring_val(elem), &d) ==
      REDISMODULE_ERR) {
    r = None;
  } else {

    dst = caml_copy_double(d);
    r = Some(dst);
  }

  CAMLreturn(r);
}

value key_zset_rem(value key, value elem) {
  CAMLparam2(key, elem);
  CAMLlocal1(r);
  int deleted;

  RedisModule_ZsetRem(Key_val(key), Rstring_val(elem), &deleted);
  r = deleted ? OK : ERR;
  CAMLreturn(r);
}

/*
ZsetIncrBy
ZsetRangeStop
ZsetRangeStop
ZsetFirstInScoreRange
ZsetLastInScoreRange
ZsetFirstInLexRange
ZsetLastInLexRange
ZsetRangeCurrentElement
ZsetRangeNext
ZsetRangePrev
ZsetRangeEndReached
*/
