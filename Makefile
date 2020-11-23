CC=gcc
CFLAGS=-g3 -O0 -fsanitize=address -fno-omit-frame-pointer -fno-optimize-sibling-calls
CLIBS=-lbsd
SRCDIR = $(PWD)/src
TESTSDIR = $(PWD)/tests
SOURCE_OBJS = $(SRCDIR)/parser.o $(SRCDIR)/ast.o $(SRCDIR)/lexer.o $(SRCDIR)/token.o
OBJS = $(SRCDIR)/repl.o $(SOURCE_OBJS)
PROG = monkey


.PHONY: all clean

all: $(PROG)

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $(CLIBS) $(OBJS) -o $(SRCDIR)/$@

repl.o: $(SRCDIR)/repl.c $(SRCDIR)/repl.h
	$(CC) $(CFLAGS) -c $(SRCDIR)/repl.c

parser.o: $(SRCDIR)/parser.c $(SRCDIR)/parser.h $(SRCDIR)/ast.o $(SRCDIR)/lexer.o
	$(CC) $(CFLAGS) -c $(SRCDIR)/parser.c

ast.o: $(SRCDIR)/ast.c $(SRCDIR)/ast.h $(SRCDIR)/token.o
	$(CC) $(CFLAGS) -c $(SRCDIR)/ast.c

lexer.o: $(SRCDIR)/lexer.c $(SRCDIR)/lexer.h $(SRCDIR)/token.o
	$(CC) $(CFLAGS) -c $(SRCDIR)/lexer.c

token.o: $(SRCDIR)/token.c $(SRCDIR)/token.h
	$(CC) $(CFLAGS) -c $(SRCDIR)/token.c

lexer_test: $(TESTSDIR)/lexer_test.c $(SRCDIR)/lexer.o
	$(CC) $(CFLAGS) $(CLIBS) $(SRCDIR)/lexer.o $(SRCDIR)/token.o $(TESTSDIR)/lexer_test.c -o $(TESTSDIR)/lexer_test

parser_test: $(TESTSDIR)/parser_test.c $(SRCDIR)/parser.o
	$(CC) $(CFLAGS) $(CLIBS) $(SOURCE_OBJS) $(TESTSDIR)/parser_test.c -o $(TESTSDIR)/parser_test

clean:
	rm -rf *~ $(SRCDIR)/*.o $(SRCDIR)/monkey $(TESTSDIR)/parser_test $(TESTSDIR)/lexer_test
