CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic -Wno-deprecated-declarations

NAME = cli

CRYPTO_DIR = ../crypto
BLOCKCHAIN_DIR = ../blockchain/v0.3
CRYPTO_LIB = $(CRYPTO_DIR)/libhblk_crypto.a
BLOCKCHAIN_LIB = $(BLOCKCHAIN_DIR)/libhblk_blockchain.a

LDLIBS = -L$(BLOCKCHAIN_DIR) -lhblk_blockchain -L$(CRYPTO_DIR) -lhblk_crypto \
	-lllist -lssl -lcrypto -pthread

SRC = main.c \
	wallet.c \
	send.c \
	mine.c \
	info.c \
	storage.c \
	utils.c
OBJ = $(SRC:.c=.o)

.PHONY: all libs clean fclean re

all: $(NAME)

$(NAME): libs $(OBJ)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ) $(LDLIBS)

libs:
	$(MAKE) -C $(CRYPTO_DIR)
	$(MAKE) -C $(BLOCKCHAIN_DIR) libhblk_blockchain.a

$(OBJ): cli.h

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all
