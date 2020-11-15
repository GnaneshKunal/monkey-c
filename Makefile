CC=gcc
CFLAGS=-g3 -O0 -fsanitize=address -fno-omit-frame-pointer -fno-optimize-sibling-calls
CLIBS=-lbsd
SOURCE_OBJS = parser.o ast.o lexer.o token.o
OBJS = repl.o $(SOURCE_OBJS)
PROG = monkey

.PHONY: all clean

all: $(PROG)

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $(CLIBS) $(OBJS) -o $@

repl.o: repl/repl.c repl/repl.h
	$(CC) $(CFLAGS) -c repl/repl.c

parser.o: parser/parser.c parser/parser.h ast.o lexer.o
	$(CC) $(CFLAGS) -c parser/parser.c

ast.o: ast/ast.c ast/ast.h token.o
	$(CC) $(CFLAGS) -c ast/ast.c

lexer.o: lexer/lexer.c lexer/lexer.h token.o
	$(CC) $(CFLAGS) -c lexer/lexer.c

token.o: token/token.c token/token.h
	$(CC) $(CFLAGS) -c token/token.c

lexer_test: lexer/lexer_test.c lexer.o
	$(CC) $(CFLAGS) $(CLIBS) lexer.o token.o lexer/lexer_test.c -o lexer_test

parser_test: parser/parser_test.c parser.o
	$(CC) $(CFLAGS) $(CLIBS) $(SOURCE_OBJS) parser/parser_test.c -o parser_test

clean:
	rm -rf *~ *.o monkey parser_test lexer_test
