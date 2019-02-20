open Redis_module

let echo ctx args =
  Reply.simple_string ctx (Rstring.to_string (Args.get args 1))

let add ctx args =
  let r =
    Rstring.to_int (Args.get args 1) + Rstring.to_int (Args.get args 2)
  in
  Reply.int ctx r

let sha256 ctx args =
  let s = Rstring.to_string (Args.get args 1) in
  let h = Digestif.digest_string Digestif.sha256 s in
  Reply.simple_string ctx (Digestif.to_hex Digestif.sha256 h)

let _ =
  on_load (fun ctx _args ->
      let _ = init ctx "hello" 1 api_version_1 in
      create_commands ctx
        [ ("hello.echo", echo, "readonly", (0, 0, 0))
        ; ("hello.add", add, "readonly", (0, 0, 0))
        ; ("hello.sha256", sha256, "readonly", (0, 0, 0)) ] )
