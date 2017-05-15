OCAMLOPT?=ocamlopt
CC?=cc
CFLAGS+=-fPIC -I`ocamlc -where`
LDFLAGS+=-L`ocamlc -where` -lasmrun_pic
OBJ=\
	redis_module_call_reply.o \
	redis_module_string.o \
	redis_module_stubs.o
MODULE?=example

# Link $(MODULE).ml with Redis_module and $(OBJ) and create shared object
mod: base
	$(OCAMLOPT) -output-obj -o $(MODULE).o -I src redis_module.cmx mod/$(MODULE)/$(MODULE).ml
	$(CC) $(CFLAGS) -shared -o mod/$(MODULE).so $(OBJ) $(MODULE).o $(CFILES) $(LDFLAGS)

# Compile OCaml Redis_module
base: $(OBJ)
	$(OCAMLOPT) -fPIC -c src/redis_module.ml -o redis_module.o

clean:
	rm -f *.o src/*.o *.cm* src/*.cm* *.so *.a src/*.so src/*.a mod/*.so

%.o: src/%.c
	mkdir -p _build/src
	$(CC) $(CFLAGS) -c src/$*.c -o $@
