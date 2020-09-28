GREEN   = \033[1;32m
WHITE   = \033[0;m


CC		= gcc
FLAGS	= -I ./include/
FLAGS	+= -std=c99 -Wall -Wextra -g
SRCS	= array.c \
		struct.c \
		main.c
OBJS    = $(SRCS:.c=.o)
NAME	= 2Darray



.PHONY: all clean run

all: $(NAME)
	@if [ -e "make" ]; then\
		rm make;\
		echo "Everything is up to date.";\
	else\
		printf "$(GREEN)'$(NAME)'$(WHITE) is up to date.\n";\
	fi


$(NAME): $(OBJS)
	@if [ ! -e "make" ]; then\
	echo >> "make";\
	fi
	
	@echo "-------------------"
	@$(CC) $(OBJS) -o $(NAME) $(FLAGS)
	@printf "[$(GREEN)OK$(WHITE)] Binary : $(NAME)\n"
	@echo "-------------------"

	
$(OBJS):
	@$(CC) $(FLAGS) -c -o $@ $<
	@printf "[$(GREEN)OK$(WHITE)] $<\n"


clean:
	rm -f $(OBJS)
	rm -f $(NAME)

run:
	./$(NAME)