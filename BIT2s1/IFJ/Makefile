CC     = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic -Werror
INCLUDE= -lm
OPTIONS=
SRC    = src/*.c
OBJ    = *.o
BUILDS = $(SRC:.c=.o)
DEBUG  = NDEBUG
EXEC   = IFJ2020



all: build clean_build

build: build_objects
	@$(CC) $(FLAGS) $(INCL) $(OBJ) -o $(EXEC)
	#-o [file name] = output file name (gcc [options] [source files] [object files] -o output file)
	#-c [file name] = generate object file [*.o] from file name (gcc -c [options] [source files]) {The -c option says not to run the linker. Then the output consists of object files outputted by the assembler.}

build_objects:
	@$(CC) -c $(SRC)

run:
	@./$(EXEC)

clean: clean_build
	@-rm -rf $(EXEC) || true

clean_build:
	@-rm -rf $(OBJ) || true
	@-rm -rf $(EXEC).core || true



# SOURCES:
# https://www.rapidtables.com/code/linux/gcc/gcc-c.html
# https://linux.die.net/man/1/gcc