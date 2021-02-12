#list of all src and object files
csrc := $(wildcard ../src/*.c)
cobj := $(csrc:.c=.o)

#directories containing the headers, libraries and executables
chead:= ../include

#flags necessary to the compilation
CC := gcc
CFLAGS:= -fPIC -Wall -Werror -Wextra -g -I$(chead) -Icstructures/include
lib_b:= 

#objects compilation from the source files
%.o: %.c
	@ echo "Building $@"
	@ $(CC) $(CFLAGS) -o $@ -c $<


#libraries compilation and linking (version number -> *.so file)
#
#	DYN libs here
#


#overall functions
all: $(lib_b)

#phony rules to build needed libraries and to clean builds
.PHONY= clean
clean:
	@ echo "cleaning libraries *.so files"
	@ rm -rf *.so* ../src/*.o