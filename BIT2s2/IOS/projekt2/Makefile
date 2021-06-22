# Makefile pro IOS Projekt 2
CC     = gcc
FLAGS  = -std=gnu99 -Wall -Wextra -Werror -pedantic
INCL   = -pthread -lrt
SRC    = src/*.c src/libs/*.c
#examples/processes_and_shared_semaphores_example.c
#OBJ    = $(SRC:.c=.o)
OBJ    = *.o
EXE    = proj2


all: build clean_build

build: build_objects
	@$(CC) $(FLAGS) $(INCL) $(OBJ) -o $(EXE)
#-o [file name] = output file name (gcc [options] [source files] [object files] -o output file)
#-c [file name] = generate object file [*.o] from file name (gcc -c [options] [source files]) {The -c option says not to run the linker. Then the output consists of object files outputted by the assembler.}

build_objects:
	@gcc -c $(SRC)

run:
	@./$(EXE)

clean: clean_build
	@-rm $(EXE)

clean_build:
	@-rm $(OBJ) 
	@-rm -rf $(EXE).core || true



# SOURCES:
# https://www.rapidtables.com/code/linux/gcc/gcc-c.html
# https://linux.die.net/man/1/gcc