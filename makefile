#directories containing the headers, libraries and executables
chead:= include
clib := lib
cbin := bin

#flags necessary to the compilation
CC := gcc
APP := app1
CFLAGS:= -fPIC -Wall -Werror -Wextra -g -I$(chead) -Ilib/cstructures/include
LFLAGS:=
LDFLAGS:= -Wl,--disable-new-dtags -Wl,-rpath,\$$ORIGIN/../lib -L$(clib)

#executables compilation
$(APP): 
		@ echo "Building $(APP)"
		@ mkdir -p bin
		@ $(CC) $(LDFLAGS) -o $(cbin)/$@ main.c $(LFLAGS) $(CFLAGS)

#overall functions
.PHONY: clean
clean:
	@ echo "cleaning binaries"
	@ rm -rf $(cbin)/*
	@ $(MAKE) -f build.mk -C$(clib) clean