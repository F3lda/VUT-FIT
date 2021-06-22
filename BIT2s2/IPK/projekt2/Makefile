CC     = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic -Werror
INCLUDE= -lm
OPTIONS=
SRC    = src/server/*.c src/libs/*.c
OBJ    = *.o
BUILDS = $(SRC:.c=.o)
DEBUG  = NDEBUG
EXEC   = ipk-simpleftp-server



all: build clean_build build2 clean_build2

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


CC2     = gcc
CFLAGS2 = -std=c99 -Wall -Wextra -pedantic -Werror
INCLUDE2= -lm
OPTIONS2=
SRC2    = src/client/*.c
OBJ2    = *.o
BUILDS2 = $(SRC2:.c=.o)
DEBUG2  = NDEBUG
EXEC2   = ipk-simpleftp-client


build2: build_objects2
	@$(CC2) $(FLAGS2) $(INCL2) $(OBJ2) -o $(EXEC2)
#-o [file name] = output file name (gcc [options] [source files] [object files] -o output file)
#-c [file name] = generate object file [*.o] from file name (gcc -c [options] [source files]) {The -c option says not to run the linker. Then the output consists of object files outputted by the assembler.}

build_objects2:
	@$(CC2) -c $(SRC2)

run2:
	@./$(EXEC2)

clean2: clean_build2
	@-rm -rf $(EXEC2) || true

clean_build2:
	@-rm -rf $(OBJ2) || true
	@-rm -rf $(EXEC2).core || true




# SOURCES:
# https://www.rapidtables.com/code/linux/gcc/gcc-c.html
# https://linux.die.net/man/1/gcc
