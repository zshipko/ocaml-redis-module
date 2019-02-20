type context

type status =
  | OK
  | ERR

type mode =
  | Read
  | Write
  | RW

type key_type =
  | Empty
  | String
  | List
  | Hash
  | Set
  | Zset
  | Module

type reply_type =
  | Unknown
  | String
  | Error
  | Integer
  | Array
  | Null

type list_flag =
  | Head
  | Tail

val api_version_1 : int

exception Wrong_arity

exception Invalid_type

module Call_reply : sig
  type t

  external proto : t -> string = "call_reply_proto"

  external to_string : t -> string option = "call_reply_to_string"

  external to_int64 : t -> int64 option = "call_reply_to_int64"

  val to_int : t -> int option

  external get_type : t -> reply_type = "call_reply_get_type"

  external length : t -> int64 = "call_reply_length"

  external get : t -> int64 -> t option = "call_reply_get"
end

module Rstring : sig
  type t

  external to_string : t -> string = "rstring_to_string"

  val to_value : t -> 'a option

  external to_int64 : t -> int64 = "rstring_to_int64"

  val to_int : t -> int

  external to_float : t -> float = "rstring_to_float"

  external from_string : context -> string -> t = "rstring_from_string"

  val from_value : context -> 'a -> t

  external from_call_reply : Call_reply.t -> t = "rstring_from_call_reply"

  val from_int64 : context -> int64 -> t

  val from_int : context -> int -> t

  val from_float : context -> float -> t

  external copy : context -> t -> t = "rstring_copy"

  external append : context -> t -> string -> status = "rstring_append"

  external compare : t -> t -> int = "rstring_compare"
end

module Args : sig
  type t

  external length : t -> int = "args_length"

  external get : t -> int -> Rstring.t = "args_get"
end

module Reply : sig
  external wrong_arity : context -> status = "reply_wrong_arity"

  external int64 : context -> int64 -> status = "reply_int64"

  external int : context -> int -> status = "reply_int"

  external simple_string : context -> string -> status = "reply_simple_string"

  external error : context -> string -> status = "reply_error"

  external null : context -> status = "reply_null"

  external array : context -> int -> status = "reply_array"

  external set_array_length :
    context -> int -> unit
    = "reply_set_array_length"

  external string : context -> string -> status = "reply_string"

  external rstring : context -> Rstring.t -> status = "reply_rstring"

  external float : context -> float -> status = "reply_float"

  external call_reply : context -> Call_reply.t -> status = "reply_call_reply"
end

type hash_flag =
  | HASH_NONE
  | HASH_NX
  | HASH_XX
  | HASH_EXISTS

module Key : sig
  type t

  external find : context -> Rstring.t -> mode -> t option = "key_find"

  external get_type : t -> key_type = "key_type"

  external length : t -> int64 = "key_length"

  external list_push : t -> list_flag -> Rstring.t -> status = "key_list_push"

  external list_pop : t -> list_flag -> Rstring.t option = "key_list_pop"

  external delete : t -> status = "key_delete"

  external set : t -> Rstring.t -> status = "key_set"

  external get_string : t -> string option = "key_get_string"

  external truncate : t -> int64 = "key_truncate"

  external get_expire : t -> int64 = "key_get_expire"

  external set_expire : t -> int64 -> status = "key_set_expire"

  external hash_get :
    t -> hash_flag -> Rstring.t array -> Rstring.t array
    = "key_hash_get"

  external hash_set :
    t -> hash_flag -> (Rstring.t * Rstring.t) array -> status
    = "key_hash_set"

  external hash_delete : unit -> Rstring.t = "key_hash_delete"

  external zset_add : t -> float -> Rstring.t -> status = "key_zset_add"

  external zset_score : t -> Rstring.t -> float option = "key_zset_score"

  val no_expire : int64
end

external init : context -> string -> int -> int -> status = "module_init"

val on_load : (context -> Args.t -> status) -> unit

val create_command :
     context
  -> string
  -> (context -> Args.t -> status)
  -> string
  -> int * int * int
  -> status

val create_commands :
     context
  -> (string * (context -> Args.t -> status) * string * (int * int * int))
     list
  -> status

external get_selected_db : context -> int = "module_get_selected_db"

external select_db : context -> int -> status = "module_select_db"

val replicate : context -> string -> string list -> status

external replicate_verbatim : context -> status = "module_replicate_verbatim"

val call : context -> string -> string list -> Call_reply.t
