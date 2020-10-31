#ifndef PARSER_H
#define PARSER_H

#include "../utils/utils.h"
#include "../ast/ast.h"
#include "../token/token.h"
#include "../lexer/lexer.h"

typedef struct _parser_t {
  lexer_t *l;
  token_t *cur_token;
  token_t *peek_token;
} parser_t;

parser_t *parser_new(lexer_t *l);
void parser_next_token(parser_t *p);
void parser_destroy(parser_t **p_p);
program_t *parser_parse_program(parser_t *p);
statement_t *parser_parse_statement(parser_t *p);
let_statement_t *parser_parse_let_statement(parser_t *p);

bool parser_cur_token_is(parser_t *parser, TOKEN token_type);
bool parser_peek_token_is(parser_t *parser, TOKEN token_type);
bool parser_expect_peek(parser_t *parser, TOKEN token_type);

#endif
