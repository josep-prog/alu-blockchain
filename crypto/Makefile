CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic -Wno-deprecated-declarations -I.
AR = ar
ARFLAGS = rcs
LDLIBS = -lssl -lcrypto

LIB = libhblk_crypto.a

SRC = sha256.c \
      ec_create.c \
      ec_to_pub.c \
      ec_from_pub.c \
      ec_save.c \
      ec_load.c \
      ec_sign.c \
      ec_verify.c

OBJ = $(SRC:.c=.o)

.PHONY: all clean fclean re

all: $(LIB)

$(LIB): $(OBJ)
	$(AR) $(ARFLAGS) $@ $^

%.o: %.c hblk_crypto.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(LIB)

re: fclean all
