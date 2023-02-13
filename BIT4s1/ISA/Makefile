CC     = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic -Werror -L/usr/local/ssl/lib $(shell pkg-config --cflags libxml-2.0)
INCLUDE= -lm -lssl -lcrypto $(shell pkg-config --libs libxml-2.0)
SRC    = src/*.c
OBJ    = *.o
BUILDS = $(SRC:.c=.o)
EXEC   = feedreader



all: build clean_build

build: build_objects
	@$(CC) $(CFLAGS) $(INCLUDE) $(OBJ) -o $(EXEC)
#-o [file name] = output file name (gcc [options] [source files] [object files] -o output file)
#-c [file name] = generate object file [*.o] from file name (gcc -c [options] [source files]) {The -c option says not to run the linker. Then the output consists of object files outputted by the assembler.}

build_objects:
	@$(CC) -c $(SRC) $(CFLAGS) $(INCLUDE)

run:
	@./$(EXEC)

clean: clean_build
	@-rm -rf $(EXEC) || true
	@-rm -rf temp_file.txt || true

clean_build:
	@-rm -rf $(OBJ) || true
	@-rm -rf $(EXEC).core || true

test:
	@bash ./tests/test.sh ./$(EXEC)

# SOURCES:
# https://www.rapidtables.com/code/linux/gcc/gcc-c.html
# https://linux.die.net/man/1/gcc
