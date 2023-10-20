CC = gcc
SRC = ku_conv.c
OBJ = $(SRC:.c=.o)
NAME = ku_conv

#%.o: %.c
#	$(CC) -c $<

$(NAME):# $(OBJ)
	$(CC) $(SRC) -o $@

.PHONY: all clean fclean re

all: $(NAME)

clean:
	rm -f $(OBJ)

fclean:
	rm -f $(OBJ) $(NAME)

re:
	make fclean
	make all
