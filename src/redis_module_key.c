#include "ocaml_redis_module.h"

#define convert_mode(_mode) Int_val(_mode) == 0 ? REDISMODULE_READ : Int_val(_mode) == 1 ? REDISMODULE_WRITE : REDISMODULE_READ | REDISMODULE_WRITE
#define convert_hash_flag(_hf) Int_val(_hf) == 1 ? REDISMODULE_HASH_NX : Int_val(_hf) == 2 ? REDISMODULE_HASH_XX :  Int_val(_hf) == 3 ? REDISMODULE_HASH_EXISTS : REDISMODULE_HASH_NONE

value key_find(value ctx, value keyname, value mode){
    return Val_key(RedisModule_OpenKey(Context_val(ctx), Value_val(keyname), convert_mode(mode)));
}

value key_type(value key){
    return Val_int(RedisModule_KeyType(Key_val(key)));
}

value key_length(value key){
    CAMLparam1(key);
    CAMLreturn(caml_copy_int64(RedisModule_ValueLength(Key_val(key))));
}

value key_list_push(value key, value where, value v){
    return Val_int(RedisModule_ListPush(Key_val(key), Int_val(where), Value_val(v)));
}

value key_list_pop(value key, value where){
    CAMLparam2(key, where);
    RedisModuleString *str = RedisModule_ListPop(Key_val(key), Int_val(where));
    if (!str){
        return None;
    }
    return Some(Val_value(str));
}

value key_delete(value key){
    return Val_int (RedisModule_DeleteKey(Key_val(key)));
}

value key_set (value key, value v){
    return Val_int (RedisModule_StringSet(Key_val(key), Value_val(v)));
}

value key_get_string (value key) {
    CAMLparam1(key);
    size_t len;
    const char *c = RedisModule_StringDMA(Key_val(key), &len, REDISMODULE_READ);
    if (!c){
        CAMLreturn(None);
    }

    CAMLlocal1(v);
    v = caml_alloc_string(len);
    memcpy(String_val(v), c, len);
    CAMLreturn(Some(v));
}

value key_truncate (value key, value size){
    CAMLparam2(key, size);
    CAMLreturn(Val_int(RedisModule_StringTruncate(Key_val(key), Int64_val(size))));
}

value key_get_expire (value key){
    CAMLparam1(key);
    CAMLreturn(caml_copy_int64(RedisModule_GetExpire(Key_val(key))));
}

value key_set_expire (value key, value i){
    return Val_int (RedisModule_SetExpire(Key_val(key), Int64_val(i)));
}

value key_hash_delete(value unit){
    return Val_value(REDISMODULE_HASH_DELETE);
}

value key_hash_set (value key, value flag, value arr){
    int len = caml_array_length(arr);
    int i;

    for (i = 0; i < len; i++){
        value v = Field(arr, i);
        if (RedisModule_HashSet(Key_val(key), convert_hash_flag(flag), Value_val(Field(v, 0)), Value_val(Field(v, 1)), NULL) != REDISMODULE_OK){
            return ERR;
        }
    }

    return OK;
}

value key_hash_get (value key, value flag, value arr){
    CAMLparam3(key, flag, arr);
    int len = caml_array_length(arr);
    int i;

    CAMLlocal1(dst);
    dst = caml_alloc(len, 0);

    RedisModuleString *s;
    for (i = 0; i < len; i++){
        value v = Field(arr, i);
        if (RedisModule_HashGet(Key_val(key), convert_hash_flag(flag), Value_val(v), &s, NULL) != REDISMODULE_OK){
            CAMLreturn(None);
        }
        Field(dst, i) = Val_value(s);
    }

    CAMLreturn(Some(dst));
}

value key_zset_add(value key, value score, value elem){
    return Val_int(RedisModule_ZsetAdd(Key_val(key), Double_val(score), Value_val(elem), NULL));
}

value key_zset_score(value key, value elem) {
    CAMLparam2(key, elem);
    double d;
    if (RedisModule_ZsetScore(Key_val(key), Value_val(elem), &d) == REDISMODULE_ERR){
        CAMLreturn(None);
    }

    CAMLreturn(Some(caml_copy_double(d)));
}

value key_zset_rem(value key, value elem) {
    int deleted;

    RedisModule_ZsetRem(Key_val(key), Value_val(elem), &deleted);

    return deleted ? OK : ERR;
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


