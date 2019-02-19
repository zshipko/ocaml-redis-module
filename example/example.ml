open Redis_module

let echo ctx args =
    Reply.simple_string ctx (Value.to_string (Args.get args 1))

let add ctx args =
    Reply.int ctx (Value.to_int (Args.get args 1) + Value.to_int (Args.get args 2))

let _ =
on_load (fun ctx _args ->
    let _ = init ctx "hello" 1 api_version_1 in
    create_commands ctx [
        "hello.echo", echo, "readonly", (0, 0, 0);
        "hello.add", add, "readonly", (0, 0, 0);
    ])
