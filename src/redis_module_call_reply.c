#include "ocaml_redis_module.h"

value module_call_internal(value ctx, value cmd, value args) {
  CAMLparam3(ctx, cmd, args);
  CAMLlocal1(r);
  r = Val_unit;

  RedisModuleCallReply *reply =
      RedisModule_Call((RedisModuleCtx *)ctx, String_val(cmd), "b",
                       String_val(args), caml_string_length(args));
  if (!reply) {
    caml_failwith("Invalid reply");
  } else {
    r = Val_call_reply(reply);
  }

  CAMLreturn(r);
}

// Call reply

value call_reply_proto(value cr) {
  CAMLparam1(cr);
  CAMLlocal1(r);

  size_t len;
  const char *s = RedisModule_CallReplyProto(Call_reply_val(cr), &len);

  r = caml_alloc_string(len);
  memcpy(String_val(r), s, len);
  CAMLreturn(r);
}

value call_reply_to_string(value cr) {
  CAMLparam1(cr);
  CAMLlocal2(r, dst);

  size_t len;
  const char *s = RedisModule_CallReplyStringPtr(Call_reply_val(cr), &len);
  if (!s) {
    r = None;
  } else {
    dst = caml_alloc_string(len);
    memcpy(String_val(dst), s, len);
    r = Some(dst);
  }
  CAMLreturn(r);
}

value call_reply_to_int64(value cr) {
  CAMLparam1(cr);
  CAMLlocal2(r, dst);

  if (RedisModule_CallReplyType(Call_reply_val(cr)) !=
      REDISMODULE_REPLY_INTEGER) {
    r = None;
  } else {
    long long n = RedisModule_CallReplyInteger(Call_reply_val(cr));
    dst = caml_copy_int64(n);
    r = Some(dst);
  }
  CAMLreturn(r);
}

value call_reply_get_type(value cr) {
  CAMLparam1(cr);
  CAMLlocal1(r);
  r = (Int_val(RedisModule_CallReplyType(Call_reply_val(cr))));
  CAMLreturn(r);
}

value call_reply_length(value cr) {
  CAMLparam1(cr);
  CAMLlocal1(r);
  r = caml_copy_int64(RedisModule_CallReplyLength(Call_reply_val(cr)));
  CAMLreturn(r);
}

value call_reply_get(value cr, value i) {
  CAMLparam2(cr, i);
  CAMLlocal1(r);

  RedisModuleCallReply *reply =
      RedisModule_CallReplyArrayElement(Call_reply_val(cr), Int64_val(i));
  if (reply == NULL) {
    r = None;
  } else {
    r = Some(Val_call_reply(reply));
  }

  CAMLreturn(r);
}
