OCAMLOPT?=ocamlopt
CC?=cc
CFLAGS+=-fPIC -I`ocamlc -where`
LDFLAGS+=-L`ocamlc -where` -lasmrun_pic
OBJ=\
	redis_module_call_reply.o \
	redis_module_string.o \
	redis_module_key.o \
	redis_module_stubs.o
MODULE_PATH?=./mod
MODULE?=example

# Link $(MODULE).ml with Redis_module and $(OBJ) and create shared object
mod: base
	$(OCAMLOPT) -output-obj -o $(MODULE).o -I src redis_module.cmx $(MODULE_PATH)/$(MODULE)/$(MODULE).ml $(MODULE_OCAML_FILES)
	$(CC) $(CFLAGS) -shared -o $(MODULE_PATH)/$(MODULE).so $(OBJ) $(MODULE).o $(MODULE_C_FILES) $(LDFLAGS)

# Compile OCaml Redis_module
base: $(OBJ)
	$(OCAMLOPT) -fPIC -c src/redis_module.ml -o redis_module.o

clean:
	rm -f *.o src/*.o *.cm* src/*.cm* *.so *.a src/*.so src/*.a $(MODULE_PATH)/*.so

%.o: src/%.c
	$(CC) $(CFLAGS) -c src/$*.c -o $@
