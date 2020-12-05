#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "lexer.h"
#include "token.h"
#include "utils.h"

typedef enum {
  LOWEST_PRECEDENCE,
  EQUALS_PRECEDENCE,
  LESSGREATER_PRECEDENCE,
  SUM_PRECEDENCE,
  PRODUCT_PRECEDENCE,
  PREFIX_PRECEDENCE,
  CALL_PRECEDENCE,
} PRECEDENCE;

/* forward declaration */
typedef struct _parser_t parser_t;

typedef expression_t *(*prefix_parse_fn)(parser_t *parser, token_t *token,
                                         PRECEDENCE precedence);
typedef expression_t *(*infix_parse_fn)(parser_t *parser, token_t *token,
                                        PRECEDENCE precedence,
                                        expression_t *left_expression);

struct _parser_t {
  lexer_t *l;
  token_t *cur_token;
  token_t *peek_token;
  char **errors;
  size_t errors_len;
  prefix_parse_fn *prefix_parselets;
  infix_parse_fn *infix_parselets;
};

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
expression_statement_t *parser_parse_expression_statement(parser_t *p);
block_statement_t *parser_parse_block_statement(parser_t *parser);
expression_t *parser_parse_expression(parser_t *p, PRECEDENCE precedence);

bool parser_cur_token_is(parser_t *parser, TOKEN token_type);
bool parser_peek_token_is(parser_t *parser, TOKEN token_type);
bool parser_expect_peek(parser_t *parser, TOKEN token_type);

expression_t *parser_parse_identifier(parser_t *parser, token_t *token,
                                      PRECEDENCE precedence);
expression_t *parser_parse_integer(parser_t *parser, token_t *token,
                                   PRECEDENCE precedence);
expression_t *parser_parse_boolean(parser_t *parser, token_t *token,
                                   PRECEDENCE precedence);
expression_t *parser_parse_grouped_expression(parser_t *parser, token_t *token,
                                              PRECEDENCE precedence);
expression_t *parser_parse_if_expression(parser_t *parser, token_t *token,
                                         PRECEDENCE precedence);
param_t *parser_parse_params(parser_t *parser);
param_exp_t *parser_parse_param_exps(parser_t *parser);
expression_t *parser_parse_fn_literal(parser_t *parser, token_t *token,
                                      PRECEDENCE precedence);

expression_t *parser_parse_prefix(parser_t *parser, token_t *token,
                                  PRECEDENCE precedence);

expression_t *parser_parse_infix(parser_t *parser, token_t *token,
                                 PRECEDENCE precedence, expression_t *left);
expression_t *parser_parse_call_expression(parser_t *parser, token_t *token,
                                           PRECEDENCE precedence, expression_t *left);

PRECEDENCE token_get_precedence(token_t *token);
PRECEDENCE parser_cur_precedence(parser_t *parser);
PRECEDENCE parser_peek_precedence(parser_t *parser);

#endif
