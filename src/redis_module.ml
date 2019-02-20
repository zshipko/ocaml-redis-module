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

let api_version_1 = 1

exception Wrong_arity

exception Invalid_type

(* Call reply *)
module Call_reply = struct
  type t

  external proto : t -> string = "call_reply_proto"

  external to_string : t -> string option = "call_reply_to_string"

  external to_int64 : t -> int64 option = "call_reply_to_int64"

  let to_int t =
    match to_int64 t with
    | Some i ->
        Some (Int64.to_int i)
    | None ->
        None

  external get_type : t -> reply_type = "call_reply_get_type"

  external length : t -> int64 = "call_reply_length"

  external get : t -> int64 -> t option = "call_reply_get"
end

module Rstring = struct
  type t

  external to_string : t -> string = "rstring_to_string"

  let to_value t =
    try Some (Marshal.from_string (to_string t) 0) with _ -> None

  let unwrap = function
    | Some x ->
        x
    | None ->
        raise Invalid_type

  let to_value_exn t = to_value t |> unwrap

  external to_int64 : t -> int64 option = "rstring_to_int64"

  let to_int64_exn t = to_int64 t |> unwrap

  let to_int s =
    match to_int64 s with
    | Some x ->
        Some (Int64.to_int x)
    | None ->
        None

  let to_int_exn t = to_int t |> unwrap

  external to_float : t -> float option = "rstring_to_float"

  let to_float_exn t = to_float t |> unwrap

  external from_string : context -> string -> t = "rstring_from_string"

  let from_value context x = from_string context (Marshal.to_string x [])

  external from_call_reply : Call_reply.t -> t = "rstring_from_call_reply"

  let from_int64 ctx i = from_string ctx (Int64.to_string i)

  let from_int ctx i = from_string ctx (string_of_int i)

  let from_float ctx f = from_string ctx (string_of_float f)

  external copy : context -> t -> t = "rstring_copy"

  external append : context -> t -> string -> status = "rstring_append"

  external compare : t -> t -> int = "rstring_compare"

  let equal a b = compare a b = 0
end

module Args = struct
  type t

  external length : t -> int = "args_length"

  external get : t -> int -> Rstring.t = "args_get"
end

(* Reply *)
module Reply = struct
  external wrong_arity : context -> status = "reply_wrong_arity"

  external int64 : context -> int64 -> status = "reply_int64"

  external int : context -> int -> status = "reply_int"

  external simple_string : context -> string -> status = "reply_simple_string"

  external error : context -> string -> status = "reply_error"

  external null : context -> status = "reply_null"

  external array : context -> ?len:int -> status = "reply_array"

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

module Key = struct
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

  let no_expire = -1L
end

(* Internal functions *)
external create_command_internal :
  context -> string -> string -> int * int * int -> status
  = "module_create_command_internal"

external call_internal :
  context -> string -> string -> Call_reply.t
  = "module_call_internal"

external replicate_internal :
  context -> string -> string -> status
  = "module_replicate_internal"

(* General module functions *)
external init : context -> string -> int -> int -> status = "module_init"

let on_load (fn : context -> Args.t -> status) =
  Callback.register_exception "Wrong_arity" Wrong_arity;
  Callback.register_exception "Invalid_type" Invalid_type;
  Callback.register "redis_module_on_load" fn

let create_command ctx name (fn : context -> Args.t -> status) flags keyinfo =
  Callback.register name (fun ctx args ->
      let x =
        try fn ctx args with
        | Wrong_arity ->
            Reply.wrong_arity ctx
        | Invalid_type ->
            Reply.error ctx "ERR invalid type"
        | Failure s ->
            Reply.error ctx ("ERR" ^ s)
        | exc ->
            Reply.error ctx ("ERR " ^ Printexc.to_string exc)
      in
      x );
  create_command_internal ctx name flags keyinfo

let create_commands ctx cmds =
  List.fold_right
    (fun (name, fn, flags, keyinfo) acc ->
      if acc = ERR then ERR else create_command ctx name fn flags keyinfo )
    cmds OK

external get_selected_db : context -> int = "module_get_selected_db"

external select_db : context -> int -> status = "module_select_db"

let replicate ctx cmd args =
  replicate_internal ctx cmd (String.concat " " args)

external replicate_verbatim : context -> status = "module_replicate_verbatim"

let call ctx cmd args = call_internal ctx cmd (String.concat " " args)
