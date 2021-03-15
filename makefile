#directories containing the headers, libraries and executables
chead:= include
clib := lib
cbin := bin

#flags necessary to the compilation
CC := gcc
CFLAGS:= -fPIC -Wall -Werror -Wextra -g -I$(chead)
LFLAGS:= -lsynchro -pthread
LDFLAGS:= -Wl,--disable-new-dtags -Wl,-rpath,\$$ORIGIN/../lib -L$(clib)

#executables compilation
prodcons: bproc 
		@ echo "Building $@"
		@ mkdir -p bin
		@ $(CC) $(LDFLAGS) -o $(cbin)/$@ $@.c $(CFLAGS) -lproc $(LFLAGS)

runners: brun
		@ echo "Building $@"
		@ mkdir -p bin
		@ $(CC) $(LDFLAGS) -o $(cbin)/$@ $@.c $(CFLAGS) -lrunner $(LFLAGS)

readerswriters: brw
		@ echo "Building $@"
		@ mkdir -p bin
		@ $(CC) $(LDFLAGS) -o $(cbin)/$@ $@.c $(CFLAGS) -lreadwrite $(LFLAGS)

#overall functions
.PHONY: all
all: prodcons runners readerswriters

.PHONY: brun
brun:
	@ $(MAKE) -f build.mk -C$(clib) lib_run

.PHONY: bproc
bproc:
	@ $(MAKE) -f build.mk -C$(clib) lib_proc

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