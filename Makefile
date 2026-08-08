NAME = codexion

CC = cc

FLAGS = -Wall -Wextra -Werror -pthread

SRC = src/burnout.c  src/clean.c  src/coders.c src/get_dongles.c src/heap.c \
		src/init.c src/main.c src/parse_args.c src/release_dongles.c  src/shared_utils.c

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
