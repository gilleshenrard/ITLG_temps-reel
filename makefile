#directories containing the headers, libraries and executables
chead:= include
clib := lib
cbin := bin

#flags necessary to the compilation
CC := gcc
CFLAGS:= -fPIC -Wall -Werror -Wextra -g -I$(chead)
LFLAGS:= -lscreen -lsynchro -pthread
LDFLAGS:= -Wl,--disable-new-dtags -Wl,-rpath,\$$ORIGIN/../lib -L$(clib)

#executables compilation
readerswriters: brw
		@ echo "Building $@"
		@ mkdir -p bin
		@ $(CC) $(LDFLAGS) -o $(cbin)/$@ $@.c $(CFLAGS) -lrwproc $(LFLAGS)

#overall functions
.PHONY: all
all: ball readerswriters

.PHONY: brw
brw:
	@ $(MAKE) -f build.mk -C$(clib) lib_rw

.PHONY: ball
ball:
	@ $(MAKE) -f build.mk -C$(clib) all

.PHONY: clean
clean:
	@ echo "cleaning binaries"
	@ rm -rf $(cbin)/*
	@ $(MAKE) -f build.mk -C$(clib) clean