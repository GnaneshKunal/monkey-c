TESTS = lexer_test parser_test evaluator_test
check_PROGRAMS = lexer_test parser_test evaluator_test
lexer_test_SOURCES = lexer_test.c $(top_builddir)/src/lexer.h
lexer_test_CFLAGS = @CHECK_CFLAGS@
lexer_test_LDADD = $(top_builddir)/src/libmonkey.la @CHECK_LIBS@

parser_test_SOURCES = parser_test.c $(top_builddir)/src/parser.h utils.h
parser_test_CFLAGS = @CHECK_CFLAGS@
parser_test_LDADD = $(top_builddir)/src/libmonkey.la @CHECK_LIBS@

evaluator_test_SOURCES = evaluator_test.c $(top_builddir)/src/evaluator.h utils.h
evaluator_test_CFLAGS = @CHECK_CFLAGS@
evaluator_test_LDADD = $(top_builddir)/src/libmonkey.la @CHECK_LIBS@
