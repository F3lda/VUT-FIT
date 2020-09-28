#**************************************************
# Makefile pro ukazkove kody do predmetu IOS na FIT VUT.
# Prace se semafory a sdilenou pameti.
# Kody jsou poskytnuty tak jak jsou.
# (C) Radek Koci, 2016
#***************************************************/

NAME = demo_sync
EXEC = $(NAME)
sources = $(NAME).c

CXX = gcc
RM = rm -f

CFLAGS = -std=gnu99 -Wall -Wextra -Werror -pedantic
LDFLAGS = -lrt -pthread

OBJFILES = $(sources:.c=.o)

.PHONY : all

all : $(EXEC) 

%.o : %.c 
	$(CXX) $(CFLAGS) -c $< -o $@

$(EXEC) : $(OBJFILES)
	$(CXX) $(CFLAGS) -o $@ $(OBJFILES) $(LDFLAGS)

clean:
	$(RM) *.o core *.out

cleanall: clean
	$(RM) $(EXEC)
