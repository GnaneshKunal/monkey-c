#include "parser.h"

parser_t *parser_new(lexer_t *l) {
  parser_t *p = malloc(sizeof(parser_t));
  assert(p);
  assert(l);
  p->l = l;
  p->cur_token = NULL;
  p->peek_token = NULL;
  p->errors = NULL;
  p->errors_len = 0;
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

    while (p->errors_len != 0) {
      free(p->errors[--p->errors_len]);
    }
    free(p->errors);

    assert(p->l);
    lexer_destroy(&p->l);
    free(p);
    *p_p = NULL;
  }
}

void parser_append_error(parser_t *parser, char *error) {
  assert(parser);
  assert(error);

  if (parser->errors == NULL) {
    parser->errors = malloc(sizeof(char *));
    parser->errors[0] = strdup(error);
    parser->errors_len = 1;
    return;
  }

  char **errors =
      realloc(parser->errors, sizeof(char *) * (parser->errors_len + 1));

  if (errors == NULL) {
    /*
     * TODO: Better error handling needed
     */
    puts("Cannot allocate memory for errors");
    exit(1);
  }
  parser->errors = errors;
  parser->errors[parser->errors_len] = strdup(error);
  parser->errors_len += 1;
}

char **parser_get_errors(parser_t *parser, size_t *error_len) {
  assert(parser);
  assert(error_len);

  *error_len = parser->errors_len;

  if (*error_len == 0)
    return NULL;

  char **errors = malloc(sizeof(char *) * (*error_len));

  size_t i = 0;
  while (i != parser->errors_len) {
    errors[i] = strdup(parser->errors[i]);
    ;
    i++;
  }
  return errors;
}

void parser_peek_error(parser_t *parser, TOKEN token_type) {
  char error[100];
  char *expected_token_str = token_to_str(token_type);
  char *actual_token_str = token_to_str(parser->peek_token->type);
  sprintf(error, "expected next token to be %s, got %s instead",
          expected_token_str, actual_token_str);
  free(expected_token_str);
  free(actual_token_str);
  parser_append_error(parser, error);
}

program_t *parser_parse_program(parser_t *parser) {

  program_t *program = program_new();

  while (parser->cur_token->type != EF) {
    statement_t *statement = parser_parse_statement(parser);
    if (statement != NULL) {
      program_append_statement(program, statement);
    }
    /*
     * We are skipping a token here and won't be using it anywhere. So
     * we must free the allocated memory of the skipping token.
     */
    token_t *tok = parser->cur_token;
    token_destroy(&tok);
    parser->cur_token = NULL;
    parser_next_token(parser);
  }

  return program;
}

statement_t *parser_parse_statement(parser_t *parser) {
  assert(parser);

  TOKEN cur_token_type = parser->cur_token->type;

  if (cur_token_type == LET) {
    let_statement_t *let_statement = parser_parse_let_statement(parser);
    return statement_new((void *)let_statement, LET_STATEMENT);
  } else if (cur_token_type == RETURN) {
    return statement_new((void *)parser_parse_return_statement(parser),
                         RETURN_STATEMENT);
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

  while (!parser_cur_token_is(parser, SEMICOLON)) {
    token_destroy(&parser->cur_token);
    parser_next_token(parser);
  }

  let_statement_t *let_statement = let_statement_new(let_token, name, value);

  return let_statement;
}

return_statement_t *parser_parse_return_statement(parser_t *parser) {
  assert(parser);

  token_t *return_token = parser->cur_token;
  expression_t *return_value = NULL;

  parser_next_token(parser);

  while (!parser_cur_token_is(parser, SEMICOLON)) {
    token_destroy(&parser->cur_token);
    parser_next_token(parser);
  }

  return return_statement_new(return_token, return_value);
}


expression_statement_t *parser_parse_expression_statement(parser_t* parser) {
  assert(parser);
  token_t *token = parser->cur_token;
  expression_t *expression = parser_parse_expression(parser);
  if (parser_peek_token_is(parser, SEMICOLON)) {
    token_destroy(&parser->cur_token);
    parser_next_token(parser);
  }
  return expression_statement_new(token, expression);
}

expression_t *parser_parse_expression(parser_t *parser) {
  /* TODO */
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
    parser_peek_error(parser, token_type);
    return false;
  }
}
