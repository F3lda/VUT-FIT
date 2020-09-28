CC=gcc
CFLAGS=-std=c99 -Wall -pedantic -Wextra
OPT=-O0 -g3
LIBS=
DEBUG=NDEBUG

main: main.o matrix.o
	${CC} ${CFLAGS} ${OPT} main.o matrix.o ${LIBS} -o main

clean:
	@-rm -rf main || true
	@-rm -rf *.o || true

main.o: main.c
	${CC} ${CFLAGS} ${OPT} -D${DEBUG} main.c ${LIBS} -o main.o -c

matrix.o: matrix.c
	${CC} ${CFLAGS} ${OPT}  -D${DEBUG} matrix.c ${LIBS} -o matrix.o -c