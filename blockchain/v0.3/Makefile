CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic -Wno-deprecated-declarations \
	-I. -Itransaction/ -Iprovided/ -I../../crypto
AR = ar
ARFLAGS = rcs

NAME = libhblk_blockchain.a
VERSIONED = libhblk_blockchain_v0.3.a

SRC = blockchain_create.c \
	block_create.c \
	block_destroy.c \
	blockchain_destroy.c \
	block_hash.c \
	blockchain_serialize.c \
	blockchain_deserialize.c \
	block_is_valid.c \
	hash_matches_difficulty.c \
	blockchain_difficulty.c \
	block_mine.c \
	transaction/tx_out_create.c \
	transaction/unspent_tx_out_create.c \
	transaction/tx_in_create.c \
	transaction/transaction_hash.c \
	transaction/tx_in_sign.c \
	transaction/transaction_create.c \
	transaction/transaction_is_valid.c \
	transaction/coinbase_create.c \
	transaction/coinbase_is_valid.c \
	transaction/transaction_destroy.c \
	transaction/update_unspent.c
OBJ = $(SRC:.c=.o)

.PHONY: all clean fclean re

all: $(NAME) $(VERSIONED)

$(NAME): $(OBJ)
	$(AR) $(ARFLAGS) $@ $(OBJ)

$(VERSIONED): $(NAME)
	cp $(NAME) $@

$(OBJ): blockchain.h transaction/transaction.h

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME) $(VERSIONED)

re: fclean all
