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

module S = String

(* Call reply *)
module Call_reply = struct
    type t
    external proto : t -> string = "call_reply_proto"
    external to_string : t -> string option = "call_reply_to_string"
    external to_int64 : t -> int64 option = "call_reply_to_int64"
    let to_int x = match to_int64 x with
        | Some i -> Some (Int64.to_int i)
        | None -> None
    external get_type : t -> reply_type = "call_reply_get_type"
    external length : t -> int64 = "call_reply_length"
    external index : t -> int64 -> t option = "call_reply_index"
end

module String = struct
    type t
    external to_string : t -> string = "rstring_to_string"
    external to_int64 : t -> int64 = "rstring_to_int64"
    let to_int s = to_int64 s |> Int64.to_int
    external to_float : t -> float = "rstring_to_float"
    external from_string : context -> string -> t = "rstring_from_string"
    external from_call_reply : Call_reply.t -> t = "rstring_from_call_reply"
    let from_int64 ctx i = from_string ctx (Int64.to_string i)
    let from_int ctx i = from_string ctx (string_of_int i)
    let from_float ctx  f = from_string ctx (string_of_float f)
    external copy : context -> t -> t = "rstring_copy"
    external append : context -> t -> string -> status = "rstring_append"
    external compare : t -> t -> int = "rstring_compare"
end

module Args = struct
    type t
    external get : t -> int -> String.t = "args_get"
    external length : t -> int = "args_length"
end

(* Reply *)
module Reply = struct
    external wrong_arity : context -> status = "reply_wrong_arity"
    external int64 : context -> int64 -> status = "reply_int64"
    external int : context -> int -> status = "reply_int"
    external simple_string : context -> string -> status = "reply_simple_string"
    external error : context -> string -> status = "reply_error"
    external null : context -> status = "reply_null"
    external array : context -> int -> status = "reply_array"
    external set_array_length : context -> int -> unit = "reply_set_array_length"
    external string : context -> string -> status = "reply_string"
    external rstring : context -> String.t -> status = "reply_rstring"
    external float : context -> float -> status = "reply_float"
    external call_reply : context -> Call_reply.t -> status = "reply_call_reply"
end

module Key = struct
    type t

    external find_rstring : context -> String.t -> t = "key_find_rstring"

    let find ctx s =
        find_rstring ctx (String.from_string ctx s)

    external get_type : t -> key_type = "key_type"
    external length : t -> int64 = "key_length"
end

(* Internal functions *)
external create_command_internal : context -> string -> string -> (int * int * int) -> status = "module_create_command_internal"
external call_internal : context -> string -> string -> Call_reply.t = "module_call_internal"
external replicate_internal : context ->  string -> string -> status = "module_replicate_internal"

(* General module functions *)
external init : context -> string -> int -> int -> status = "module_init"

let on_load (fn : context -> Args.t -> status) =
    Callback.register "redis_module_on_load" fn

let create_command ctx name (fn : context -> Args.t -> status) flags keyinfo =
    Callback.register name (fun ctx args -> let x = fn ctx args in Gc.major (); Gc.minor (); x);
    create_command_internal ctx name flags keyinfo

external get_selected_db : context -> int = "module_get_selected_db"

external select_db : context -> int -> status = "module_select_db"

let replicate ctx cmd args =
    replicate_internal ctx cmd (S.concat " " args)

external replicate_verbatim : context -> status = "module_replicate_verbatim"

let call ctx cmd args =
    call_internal ctx cmd (S.concat " " args)

