CFLAGS = -std=c99 -Wall -Wextra -Werror
LDFLAGS= -lm
EXEC   = main


.PHONY: run clean  
#a phony target is simply a target that is always out-of-date
#https://stackoverflow.com/questions/2145590/what-is-the-purpose-of-phony-in-a-makefile?rq=1


${EXEC}: ${EXEC}.o

clean:
	-rm ${EXEC} ${EXEC}.o
run:
	./${EXEC}