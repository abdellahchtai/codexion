NAME = codexion

CC = cc

FLAGS = -Wall -Wextra -Werror -fsanitize=thread -g

SRC = burnout.c  clean.c  coders.c  codexion.h  get_dongles.c  \
	heap.c  init.c  main.c  Makefile  parse_args.c  release_dongles.c  shared_utils.c

OBJ = burnout.o  clean.o  coders.o  codexion.h  get_dongles.o  \
	heap.o  init.o  main.o  Makefile  parse_args.o  release_dongles.o  shared_utils.o

all: $(NAME)

NAME: $(SRC)