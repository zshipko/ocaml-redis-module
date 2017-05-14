open Redis_module

let echo ctx args =
    if Array.length args < 2 then Reply.wrong_arity ctx
    else Reply.simple_string ctx (String.to_string args.(1))

let _ =
on_load (fun ctx args ->
    let _ = init ctx "hello" 1 api_version_1 in
    create_command ctx "hello.echo" echo "readonly" (0, 0, 0))
