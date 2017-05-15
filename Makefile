OCAMLOPT?=ocamlopt
CC?=cc
MODULE?=example
CFLAGS+=-fPIC -I`ocamlc -where`
LDFLAGS+=-L`ocamlc -where` -lasmrun_pic
OBJ=\
	redis_module_call_reply.o \
	redis_module_string.o 	  \
	redis_module_stubs.o

lib: $(OBJ)
	$(OCAMLOPT) $(OCAMLFLAGS) -I src -output-obj src/redis_module.ml $(OCAMLFILES) mod/$(MODULE)/$(MODULE).ml -o redis_module.o
	$(CC) $(CFLAGS) -shared -o mod/$(MODULE).so  redis_module.o redis_module_call_reply.o redis_module_string.o redis_module_stubs.o $(CFILES) $(LDFLAGS)

clean:
	rm -f *.o src/*.o *.cm* src/*.cm* *.so *.a src/*.so src/*.a

%.o: src/%.c
	$(CC) $(CFLAGS) -c src/$*.c -o $@
