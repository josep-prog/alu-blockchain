CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic -I.

NAME = libhblk_blockchain_v0.1.a

SRC = blockchain_create.c \
	block_create.c \
	block_destroy.c \
	blockchain_destroy.c \
	block_hash.c \
	blockchain_serialize.c \
	blockchain_deserialize.c \
	block_is_valid.c \
	provided/_endianness.c
OBJ = $(SRC:.c=.o)

.PHONY: all clean fclean re

all: $(NAME)

$(NAME): $(OBJ)
	ar rcs $(NAME) $(OBJ)

$(OBJ): blockchain.h

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all
