#include "ocaml_redis_module.h"

#define convert_mode(_mode) Int_val(_mode) == 0 ? REDISMODULE_READ : Int_val(_mode) == 1 ? REDISMODULE_WRITE : REDISMODULE_READ | REDISMODULE_WRITE

value key_find_rstring(value ctx, value keyname, value mode){
    return Val_key(RedisModule_OpenKey(Context_val(ctx), Rstring_val(keyname), convert_mode(mode)));
}

value key_type(value key){
    return Val_int(RedisModule_KeyType(Key_val(key)));
}

value key_length(value key){
    CAMLparam1(key);
    CAMLreturn(caml_copy_int64(RedisModule_ValueLength(Key_val(key))));
}
