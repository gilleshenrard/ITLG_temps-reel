#list of all src and object files
csrc := $(wildcard ../src/*.c)
cobj := $(csrc:.c=.o)

#directories containing the headers, libraries and executables
chead:= ../include

#flags necessary to the compilation
CC := gcc
CFLAGS:= -fPIC -Wall -Werror -Wextra -g -I$(chead)
lib_b:= libsynchro.so libreader.so

#objects compilation from the source files
%.o: %.c
	@ echo "Building $@"
	@ $(CC) $(CFLAGS) -o $@ -c $<


#libraries compilation and linking (version number -> *.so file)
libsynchro.so : ../src/synchro.o
	@ echo "Building $@"
	@ $(CC) -shared -fPIC -lc -Wl,-soname,$@.1 -o $@.1.0 $< -pthread
	@ ldconfig -n . -l $@.1.0
	@ ln -sf $@.1 $@

libreader.so : ../src/reader.o
	@ echo "Building $@"
	@ $(CC) -shared -fPIC -lc -Wl,-soname,$@.1 -o $@.1.0 $< -pthread
	@ ldconfig -n . -l $@.1.0
	@ ln -sf $@.1 $@

#overall functions
all: $(lib_b)

#phony rules to build needed libraries and to clean builds
.PHONY= clean
clean:
	@ echo "cleaning libraries *.o and *.so files"
	@ rm -rf *.so* ../src/*.o