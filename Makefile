OCAMLOPT?=ocamlopt
CC?=cc
MODULE?=example

lib:
	$(CC) $(CFLAGS) -fPIC -I`ocamlc -where` -fPIC -o redis_module_call_reply.o -c src/redis_module_call_reply.c
	$(CC) $(CFLAGS) -fPIC -I`ocamlc -where` -fPIC -o redis_module_string.o -c src/redis_module_string.c
	$(CC) $(CFLAGS) -fPIC -I`ocamlc -where` -fPIC -o redis_module_stubs.o -c src/redis_module_stubs.c
	$(OCAMLOPT) $(OCAMLFLAGS) -I src -output-obj src/redis_module.ml $(OCAMLFILES) mod/$(MODULE)/$(MODULE).ml -o redis_module.o
	$(CC) $(CFLAGS) -shared -o mod/$(MODULE).so  redis_module.o redis_module_call_reply.o redis_module_string.o redis_module_stubs.o -L"`ocamlc -where`" $(CFILES) -lasmrun_pic -lcurses $(LDFLAGS)

clean:
	rm -f *.o src/*.o *.cm* src/*.cm* *.so *.a
