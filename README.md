# redis-module

`redis-module` provides bindings to the [redis](https://github.com/antirez/redis) module interface in OCaml.

## Installation

Using [opam](https://github.com/ocaml/opam):

```shell
$ opam pin add redis-module git+https://github.com/zshipko/ocaml-redis-module
```

## Usage

To use `redis-module`

## Examples

To build and run the example module run:

```shell
$ dune build example/example.so
$ redis-server --loadmodule ./_build/default/example/example.so
````

See `example/example.ml` for mor information
