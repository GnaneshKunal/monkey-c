CC=gcc
CFLAGS=-g3 -O -rdynamic -fsanitize=address

.PHONY: token lexer lexer_test all

HEADERS = utils/utils.h token/token.h lexer/lexer.h

lexer_test:
	$(CC) $(CFLAGS) -g3 -O0 -rdynamic -fsanitize=address token/token.c lexer/lexer.c lexer/lexer_test.c

clean:
		-rm -f lexer_test.o

all:
	$(CC) $(token) $(lexer)
