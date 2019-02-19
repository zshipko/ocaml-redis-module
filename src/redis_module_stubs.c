#include "ocaml_redis_module.h"

const value OK = Val_int(0);
const value ERR = Val_int(1);

value Some(value x) {
  value dst = caml_alloc_small(1, 0);
  Store_field(dst, 0, x);
  return dst;
}

const value None = Val_int(0);

// Arguments to initialize OCaml instance with
const char *default_args[] = {"redis-server", NULL};

value args_get(value a, value i) {
  CAMLparam2(a, i);
  CAMLlocal1(r);
  r = Val_unit;

  Args *args = Args_val(a);

  if (args->argc > Int_val(i)) {
    r = Val_value(args->arg[Int_val(i)]);
  } else {
    caml_raise(*caml_named_value("Wrong_arity"));
  }

  CAMLreturn(r);
}

value args_length(value a) {
  CAMLparam1(a);
  CAMLlocal1(r);
  Args *args = Args_val(a);
  r = Val_int(args->argc);
  CAMLreturn(r);
}

static int internalNamedCallback(const char *name, RedisModuleCtx *ctx,
                                 value args) {
  value *fn = caml_named_value(name);
  if (!fn) {
    return REDISMODULE_ERR;
  }

  return Int_val(caml_callback2(*fn, Val_context(ctx), args));
}

// Wrap OCaml callbacks
static int internalCommandWrapper(RedisModuleCtx *ctx, RedisModuleString **argv,
                                  int argc) {
  RedisModule_AutoMemory(ctx);

  size_t cmdSize = 0;
  const char *cmd = RedisModule_StringPtrLen(argv[0], &cmdSize);
  if (!cmd) {
    return REDISMODULE_ERR;
  }

  Args args = {argc, argv};
  return internalNamedCallback(cmd, ctx, Val_args(&args));
}

// Define generic RedisModule_OnLoad
int RedisModule_OnLoad(RedisModuleCtx *ctx, RedisModuleString **argv,
                       int argc) {
  caml_startup((char **)default_args);

  Args args = {argc, argv};
  return internalNamedCallback("redis_module_on_load", ctx, Val_args(&args));
}

value module_init(value _ctx, value name, value ver, value api_ver) {
  CAMLparam4(_ctx, name, ver, api_ver);
  CAMLlocal1(r);
  RedisModuleCtx *ctx = (RedisModuleCtx *)_ctx;
  r = Val_int(
      RedisModule_Init(ctx, String_val(name), Int_val(ver), Int_val(api_ver)));
  CAMLreturn(r);
}

value module_create_command_internal(value _ctx, value name, value flags,
                                     value keyinfo) {
  CAMLparam4(_ctx, name, flags, keyinfo);
  CAMLlocal1(r);

  const char *cmd = String_val(name);
  value *fn = caml_named_value(cmd);
  if (!fn) {
    r = (value)REDISMODULE_ERR;
  } else {
    r = ((value)RedisModule_CreateCommand(
        Context_val(_ctx), cmd, internalCommandWrapper, String_val(flags),
        Int_val(Field(keyinfo, 0)), Int_val((Field(keyinfo, 1))),
        Int_val(Field(keyinfo, 2))));
  }
  CAMLreturn(r);
}

value module_get_selected_db(value ctx) {
  CAMLparam1(ctx);
  CAMLlocal1(r);
  r = (Val_int(RedisModule_GetSelectedDb(Context_val(ctx))));
  CAMLreturn(r);
}

value module_select_db(value ctx, value db) {
  CAMLparam2(ctx, db);
  CAMLlocal1(r);
  r = (Val_int(RedisModule_SelectDb(Context_val(ctx), Int_val(db))));
  CAMLreturn(r);
}

value module_replicate_internal(value ctx, value cmd, value args) {
  CAMLparam3(ctx, cmd, args);
  CAMLlocal1(r);
  r = (RedisModule_Replicate(Context_val(ctx), String_val(cmd), "b",
                             String_val(args), caml_string_length(args)));
  CAMLreturn(r);
}

value module_replicate_verbatim(value ctx) {
  CAMLparam1(ctx);
  CAMLlocal1(r);
  r = (Val_int(RedisModule_ReplicateVerbatim(Context_val(ctx))));
  CAMLreturn(r);
}

// Reply
value reply_wrong_arity(value ctx) {
  CAMLparam1(ctx);
  CAMLlocal1(r);
  r = (Val_int(RedisModule_WrongArity(Context_val(ctx))));
  CAMLreturn(r);
}

value reply_int64(value _ctx, value i) {
  CAMLparam2(_ctx, i);
  CAMLlocal1(r);
  r = Val_int(RedisModule_ReplyWithLongLong(Context_val(_ctx), Int64_val(i)));
  CAMLreturn(r);
}

value reply_int(value _ctx, value i) {
  CAMLparam2(_ctx, i);
  CAMLlocal1(r);
  r = Val_int(RedisModule_ReplyWithLongLong(Context_val(_ctx), Int_val(i)));
  CAMLreturn(r);
}

value reply_simple_string(value _ctx, value s) {
  CAMLparam2(_ctx, s);
  CAMLlocal1(r);
  r = Val_int(
      RedisModule_ReplyWithSimpleString(Context_val(_ctx), String_val(s)));
  CAMLreturn(r);
}

value reply_error(value _ctx, value s) {
  CAMLparam2(_ctx, s);
  CAMLlocal1(r);
  r = Val_int(RedisModule_ReplyWithError(Context_val(_ctx), String_val(s)));
  CAMLreturn(r);
}

value reply_null(value _ctx) {
  CAMLparam1(_ctx);
  CAMLlocal1(r);
  r = Val_int(RedisModule_ReplyWithNull(Context_val(_ctx)));
  CAMLreturn(r);
}

value reply_array(value _ctx, value i) {
  CAMLparam2(_ctx, i);
  CAMLlocal1(r);
  r = Val_int(RedisModule_ReplyWithArray(Context_val(_ctx), Int_val(i)));
  CAMLreturn(r);
}

value reply_set_array_length(value _ctx, value i) {
  CAMLparam2(_ctx, i);
  RedisModule_ReplySetArrayLength(Context_val(_ctx), Int_val(i));
  CAMLreturn(Val_unit);
}

value reply_string(value _ctx, value s) {
  CAMLparam2(_ctx, s);
  CAMLlocal1(r);
  r = (Val_int(RedisModule_ReplyWithStringBuffer(
      Context_val(_ctx), String_val(s), caml_string_length(s))));
  CAMLreturn(r);
}

value reply_value(value _ctx, value s) {
  CAMLparam2(_ctx, s);
  CAMLlocal1(r);
  r = (Val_int(RedisModule_ReplyWithString(Context_val(_ctx), Value_val(s))));
  CAMLreturn(r);
}

value reply_float(value _ctx, value f) {
  CAMLparam2(_ctx, f);
  CAMLlocal1(r);
  r = (Val_int(RedisModule_ReplyWithDouble(Context_val(_ctx), Double_val(f))));
  CAMLreturn(r);
}

value reply_call_reply(value _ctx, value reply) {
  CAMLparam2(_ctx, reply);
  CAMLlocal1(r);
  r = (Val_int(RedisModule_ReplyWithCallReply(Context_val(_ctx),
                                              Call_reply_val(reply))));
  CAMLreturn(r);
}
