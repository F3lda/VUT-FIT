CC     = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic -Werror
INCLUDE= -lm
OPTIONS=
SRC    = src/
OBJ    = *.o
BUILDS = $(SRC:.c=.o)
DEBUG  = NDEBUG
EXEC   = proj2



all: build

build: proj2.o
	${CC} ${CFLAGS} ${OPTIONS} ${INCLUDE} -D${DEBUG} ${OBJ} -o ${EXEC}

run:
	./${EXEC}

clean:
	@-rm -rf ${EXEC} || true
	@-rm -rf *.o || true



proj2.o:
	${CC} ${CFLAGS} ${OPTIONS} ${INCLUDE} -D${DEBUG} ${SRC}proj2.c -o proj2.o -c