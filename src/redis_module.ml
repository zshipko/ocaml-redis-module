type context

type status =
    | OK
    | ERR

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
    external get_type : t -> int = "call_reply_get_type"
    external length : t -> int64 = "call_reply_length"
    external index : t -> int64 -> t option = "call_reply_index"
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
    external float : context -> float -> status = "reply_float"
    external call_reply : context -> Call_reply.t -> status = "reply_call_reply"
end

module Key = struct
    type t
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

(* Internal functions *)
external create_command_internal : context -> string -> string -> (int * int * int) -> status = "module_create_command_internal"
external call_internal : context -> string -> string -> Call_reply.t = "module_call_internal"
external replicate_internal : context ->  string -> string -> status = "module_replicate_internal"

(* General module functions *)
external init : context -> string -> int -> int -> status = "module_init"

let on_load (fn : context -> String.t list -> status) =
    Callback.register "redis_module_on_load" fn

let create_command ctx name (fn : context -> String.t array -> status) flags keyinfo =
    Callback.register name fn;
    create_command_internal ctx name flags keyinfo

external get_selected_db : context -> int = "module_get_selected_db"

external select_db : context -> int -> status = "module_select_db"

let replicate ctx cmd args =
    replicate_internal ctx cmd (S.concat " " args)

external replicate_verbatim : context -> status = "module_replicate_verbatim"

let call ctx cmd args =
    call_internal ctx cmd (S.concat " " args)

let echo ctx args =
    if Array.length args < 2 then Reply.wrong_arity ctx
    else Reply.simple_string ctx (String.to_string args.(1))

let _ =
on_load (fun ctx args ->
    let _ = init ctx "hello" 1 api_version_1 in
    create_command ctx "hello.echo" echo "readonly" (0, 0, 0))
