#directories containing the headers, libraries and executables
chead:= include
clib := lib
cbin := bin

#flags necessary to the compilation
CC := gcc
CFLAGS:= -fPIC -Wall -Werror -Wextra -g -I$(chead)
LFLAGS:= -lsynchro -lproc -pthread
LDFLAGS:= -Wl,--disable-new-dtags -Wl,-rpath,\$$ORIGIN/../lib -L$(clib)

#executables compilation
prodcons: blib 
		@ echo "Building $@"
		@ mkdir -p bin
		@ $(CC) $(LDFLAGS) -o $(cbin)/$@ $@.c $(CFLAGS) $(LFLAGS)

#overall functions
.PHONY: blib
blib:
	@ $(MAKE) -f build.mk -C$(clib) all

.PHONY: clean
clean:
	@ echo "cleaning binaries"
	@ rm -rf $(cbin)/*
	@ $(MAKE) -f build.mk -C$(clib) clean