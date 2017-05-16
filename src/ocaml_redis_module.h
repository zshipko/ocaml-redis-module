#ifndef __OCAML_REDIS_MODULE__
#define __OCAML_REDIS_MODULE__

#include "redismodule.h"

#include <caml/mlvalues.h>
#include <caml/alloc.h>
#include <caml/intext.h>
#include <caml/fail.h>
#include <caml/callback.h>
#include <caml/memory.h>
#include <stdio.h>
#include <string.h>

extern value Some(value);
extern const value None;

extern const value OK;
extern const value ERR;

typedef struct Args {
    int argc;
    RedisModuleString **arg;
} Args;

#define Args_val(x) ((Args*)x)
#define Val_args(x) ((value)x)

#define Context_val(x) ((RedisModuleCtx*)x)
#define Val_context(x) ((value)x)

#define Call_reply_val(x) ((RedisModuleCallReply*)x)
#define Val_call_reply(x) ((value)x)

#define Rstring_val(x) ((RedisModuleString*)x)
#define Val_rstring(x) ((value)x)

#define Key_val(x) ((RedisModuleKey*)x)
#define Val_key(x) ((value)x)

#endif
