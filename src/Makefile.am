noinst_LTLIBRARIES = libmonkey.la
libmonkey_la_SOURCES = \
	utils.h \
	token.h \
	token.c \
	repl.h \
	repl.c \
	parser.h \
	parser.c \
	main.c \
	lexer.h \
	lexer.c \
	ast.h \
	ast.c
libmonkey_la_CFLAGS = -g3 -O0 -fsanitize=address -fno-omit-frame-pointer -fno-optimize-sibling-calls

bin_PROGRAMS = monkey
monkey_SOURCES = main.c
monkey_CPPFLAGS = -I$(top_srcdir)/src
monkey_LDADD = libmonkey.la
monkey_CFLAGS = -g3 -O0 -fsanitize=address -fno-omit-frame-pointer -fno-optimize-sibling-calls
