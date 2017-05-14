# ocaml-redis-module

`ocaml-redis-module` provides an interface for building [Redis](https://github.com/antirez/redis) modules in OCaml.

## Building

    make MODULE=example

## Running

    redis-server --loadmodule ./mod/example.so

## Example

See `mod/example/example.ml`
