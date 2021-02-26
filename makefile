#directories containing the headers, libraries and executables
chead:= include
clib := lib
cbin := bin

#flags necessary to the compilation
CC := gcc
APP := prodcons
CFLAGS:= -fPIC -Wall -Werror -Wextra -g -I$(chead)
LFLAGS:= -lsynchro -lproc -pthread
LDFLAGS:= -Wl,--disable-new-dtags -Wl,-rpath,\$$ORIGIN/../lib -L$(clib)

#executables compilation
$(APP): blib 
		@ echo "Building $(APP)"
		@ mkdir -p bin
		@ $(CC) $(LDFLAGS) -o $(cbin)/$@ main.c $(CFLAGS) $(LFLAGS)

#overall functions
.PHONY: blib
blib:
	@ $(MAKE) -f build.mk -C$(clib) all

.PHONY: clean
clean:
	@ echo "cleaning binaries"
	@ rm -rf $(cbin)/*
	@ $(MAKE) -f build.mk -C$(clib) clean