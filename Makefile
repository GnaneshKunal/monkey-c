CC=gcc
CFLAGS=-g3 -O0 -rdynamic -fsanitize=address

.PHONY: token lexer lexer_test repl all

HEADERS = utils/utils.h token/token.h lexer/lexer.h

lexer_test:
	$(CC) $(CFLAGS) token/token.c lexer/lexer.c lexer/lexer_test.c

repl:
	$(CC) $(CFLAGS) token/token.c lexer/lexer.c repl/repl.c

clean:
		-rm -f lexer_test.o
		-rm -f repl.o

all:
	$(CC) $(token) $(lexer)
