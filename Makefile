OCAMLC?=ocamlc
CC?=cc
MODULE?=example

lib:
	$(CC) $(CFLAGS) -fPIC -I`ocamlc -where` -fPIC -o redis_module_call_reply.o -c src/redis_module_call_reply.c
	$(CC) $(CFLAGS) -fPIC -I`ocamlc -where` -fPIC -o redis_module_string.o -c src/redis_module_string.c
	$(CC) $(CFLAGS) -fPIC -I`ocamlc -where` -fPIC -o redis_module_stubs.o -c src/redis_module_stubs.c
	$(OCAMLC) $(OCAML_FLAGS) -I src -output-obj src/redis_module.ml mod/$(MODULE)/$(MODULE).ml -o redis_module.o
	$(CC) $(CFLAGS) -shared -o mod/$(MODULE).so  redis_module.o redis_module_call_reply.o redis_module_string.o redis_module_stubs.o -L"`ocamlc -where`" -lcamlrun_pic -lcurses $(LDFLAGS)

lib-OLD:
	$(CC) -fPIC -I`ocamlc -where` -fPIC -o redis_module_call_reply.o -c src/redis_module_call_reply.c
	$(CC) -fPIC -I`ocamlc -where` -fPIC -o redis_module_string.o -c src/redis_module_string.c
	$(CC) -fPIC -I`ocamlc -where` -fPIC -o redis_module_stubs.o -c src/redis_module_stubs.c
	$(OCAMLC) -output-obj src/redis_module.ml -o redis_module.o
	$(CC) -shared -o ocaml_redis_module.so  redis_module.o redis_module_call_reply.o redis_module_string.o redis_module_stubs.o -L"`ocamlc -where`" -lcamlrun_pic -lcurses

clean:
	rm -f *.o src/*.o *.cm* src/*.cm* *.so *.a
