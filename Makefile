CC=gcc
CFLAGS=-g3 -O -rdynamic -fsanitize=address

.PHONY: token lexer lexer_test all

HEADERS = utils.h token.h lexer.h

lexer_test:
	$(CC) $(CFLAGS) -g3 -O0 -rdynamic -fsanitize=address token.c lexer.c lexer_test.c

clean:
		-rm -f lexer_test.o

all:
	$(CC) $(token) $(lexer)
