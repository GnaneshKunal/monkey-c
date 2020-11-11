CC=gcc
CFLAGS=-g3 -O0 -rdynamic -fsanitize=address -fno-omit-frame-pointer -fno-optimize-sibling-calls

.PHONY: token lexer lexer_test repl parser_test ast_test all

HEADERS = utils/utils.h token/token.h lexer/lexer.h ast/ast.h parser/parser.h

lexer_test:
	$(CC) $(CFLAGS) token/token.c lexer/lexer.c lexer/lexer_test.c

parser_test:
	$(CC) $(CFLAGS) token/token.c lexer/lexer.c ast/ast.c parser/parser.c parser/parser_test.c

ast_test:
	$(CC) $(CFLAGS) token/token.c lexer/lexer.c ast/ast.c ast/ast_test.c

repl:
	$(CC) $(CFLAGS) token/token.c lexer/lexer.c ast/ast.c parser/parser.c repl/repl.c

tags:
	find -type f -name "*.[ch]" -print0 | xargs -0 etags -o TAGS -a -l c

clean:
		-rm -f lexer_test.o
		-rm -f parser_test.o
		-rm -f ast_test.o
		-rm -f repl.o

all:
	$(CC) $(token) $(lexer)
