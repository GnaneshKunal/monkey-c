#ifndef PARSER_H
#define PARSER_H

#include "../ast/ast.h"
#include "../lexer/lexer.h"
#include "../token/token.h"
#include "../utils/utils.h"

typedef struct _parser_t {
  lexer_t *l;
  token_t *cur_token;
  token_t *peek_token;
  char **errors;
  size_t errors_len;
} parser_t;

parser_t *parser_new(lexer_t *l);
void parser_next_token(parser_t *p);
void parser_destroy(parser_t **p_p);
void parser_append_error(parser_t *parser, char *error);
char **parser_get_errors(parser_t *parser, size_t *error_len);
void parser_peek_error(parser_t *parser, TOKEN token_type);
program_t *parser_parse_program(parser_t *p);
statement_t *parser_parse_statement(parser_t *p);
let_statement_t *parser_parse_let_statement(parser_t *p);
return_statement_t *parser_parse_return_statement(parser_t *p);

bool parser_cur_token_is(parser_t *parser, TOKEN token_type);
bool parser_peek_token_is(parser_t *parser, TOKEN token_type);
bool parser_expect_peek(parser_t *parser, TOKEN token_type);

#endif

