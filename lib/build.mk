#list of all src and object files
csrc := $(wildcard ../src/*.c)
cobj := $(csrc:.c=.o)

#directories containing the headers, libraries and executables
chead:= ../include

#flags necessary to the compilation
CC := gcc
CFLAGS:= -fPIC -Wall -Werror -Wextra -g -I$(chead)

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

libproc.so : ../src/processes.o
	@ echo "Building $@"
	@ $(CC) -shared -fPIC -lc -Wl,-soname,$@.1 -o $@.1.0 $< -pthread
	@ ldconfig -n . -l $@.1.0
	@ ln -sf $@.1 $@

librunner.so : ../src/runner.o
	@ echo "Building $@"
	@ $(CC) -shared -fPIC -lc -Wl,-soname,$@.1 -o $@.1.0 $< -pthread
	@ ldconfig -n . -l $@.1.0
	@ ln -sf $@.1 $@

librwproc.so : ../src/rwprocess.o
	@ echo "Building $@"
	@ $(CC) -shared -fPIC -lc -Wl,-soname,$@.1 -o $@.1.0 $< -pthread
	@ ldconfig -n . -l $@.1.0
	@ ln -sf $@.1 $@

#phony rules to build needed libraries and to clean builds
.PHONY= lib_run
lib_run: libsynchro.so librunner.so

.PHONY= lib_proc
lib_proc: libsynchro.so libproc.so

.PHONY= lib_rw
lib_rw: libsynchro.so librwproc.so

.PHONY= all
all: libsynchro.so libproc.so librunner.so librwproc.so

.PHONY= clean
clean:
	@ echo "cleaning libraries *.o and *.so files"
	@ rm -rf *.so* ../src/*.o