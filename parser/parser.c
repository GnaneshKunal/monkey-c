#include "parser.h"

parser_t *parser_new(lexer_t *l) {
  parser_t *p = malloc(sizeof(parser_t));
  assert(p);
  assert(l);
  p->l = l;
  p->cur_token = NULL;
  p->peek_token = NULL;

  parser_next_token(p);
  parser_next_token(p);

  return p;
}

/*
 * If `parser_next_token` is called just to skip a token, destroy the
 * token (parser.cur_token) before calling `parser_next_token`.
 */
void parser_next_token(parser_t *p) {
  assert(p);
  p->cur_token = p->peek_token;
  p->peek_token = lexer_next_token(p->l);
}

void parser_destroy(parser_t **p_p) {
  assert(p_p);
  if (*p_p) {
    parser_t *p = *p_p;
    assert(p);
    if (p->cur_token != NULL) {
      token_destroy(&p->cur_token);
    }
    if (p->peek_token != NULL) {
      token_destroy(&p->peek_token);
    }
    assert(p->l);
    lexer_destroy(&p->l);
    free(p);
    *p_p = NULL;
  }
}

program_t *parser_parse_program(parser_t *parser) {

  program_t *program = program_new();

  while (parser->cur_token->type != EF) {
    statement_t *statement = parser_parse_statement(parser);
    if (statement != NULL) {
      program_append_statement(program, statement);
    }
    token_t *tok = parser->cur_token;
    if (tok->type == SEMICOLON) {
      token_destroy(&tok);
      parser->cur_token = NULL;
      parser_next_token(parser);
    }
  }

  return program;
}

statement_t *parser_parse_statement(parser_t *parser) {
  assert(parser);

  TOKEN cur_token_type = parser->cur_token->type;

  if (cur_token_type == LET) {
    let_statement_t *let_statement = parser_parse_let_statement(parser);
    return statement_new((void *)let_statement, LET_STATEMENT);
  } else {
    return NULL;
  }
}

let_statement_t *parser_parse_let_statement(parser_t *parser) {
  assert(parser);

  token_t *let_token = parser->cur_token;

  if (!parser_expect_peek(parser, IDENT)) {
    return NULL;
  }

  identifier_t *name =
      identifier_new(parser->cur_token, parser->cur_token->literal);

  if (!parser_expect_peek(parser, ASSIGN)) {
    return NULL;
  }

  /*
   * TODO: We're skipping the expressions until we encounter a semicolon;
   */

  expression_t *value = NULL;

  token_t *tok;
  while (!parser_cur_token_is(parser, SEMICOLON)) {
    tok = parser->cur_token;
    token_destroy(&tok);
    parser->cur_token = NULL;
    parser_next_token(parser);
  }

  token_destroy(&tok);

  let_statement_t *let_statement = let_statement_new(let_token, name, value);

  return let_statement;
}

bool parser_cur_token_is(parser_t *parser, TOKEN token_type) {
  assert(parser);
  return parser->cur_token->type == token_type;
}

bool parser_peek_token_is(parser_t *parser, TOKEN token_type) {
  assert(parser);
  return parser->peek_token->type == token_type;
}

bool parser_expect_peek(parser_t *parser, TOKEN token_type) {
  if (parser_peek_token_is(parser, token_type)) {
    parser_next_token(parser);
    return true;
  } else {
    return false;
  }
}
