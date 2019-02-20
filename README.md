# redis-module

`redis-module` provides bindings to the [redis](https://github.com/antirez/redis) module interface in OCaml.

## Installation

Using [opam](https://github.com/ocaml/opam):

```shell
$ opam pin add redis-module git+https://github.com/zshipko/ocaml-redis-module
```
## Usage

To use `redis-module` you need to build your project as a shared object. This can be done easily using [dune](https://github.com/ocaml/dune). The following is an example of the most basic working configuration:

```
(executable
  (name example)
  (modes shared_object)
  (libraries redis-module))
```

You can then use `dune build $MODULE_PATH/example.so`, where `$MODULE_PATH` is the path to your OCaml source and `dune` file.


## Example

To build and run the example module run:

```shell
$ dune runtest --no-buffer
```

See `example/example.ml` for mor information
