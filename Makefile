NAME = codexion

CC = cc

FLAGS = -Wall -Wextra -Werror -pthread

SRC = burnout.c  clean.c  coders.c get_dongles.c heap.c \
		init.c main.c parse_args.c release_dongles.c  shared_utils.c

OBJ = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
		$(CC) $(FLAGS) $(OBJ) -o $(NAME)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
