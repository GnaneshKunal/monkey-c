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

  p->prefix_parselets = calloc(RETURN_TOKEN + 1, sizeof(prefix_parse_fn));
  assert(p->prefix_parselets);
  p->prefix_parselets[IDENT_TOKEN] = parser_parse_identifier;
  p->prefix_parselets[INT_TOKEN] = parser_parse_integer;
  p->prefix_parselets[BANG_TOKEN] = parser_parse_prefix;
  p->prefix_parselets[MINUS_TOKEN] = parser_parse_prefix;
  p->prefix_parselets[TRUE_TOKEN] = parser_parse_boolean;
  p->prefix_parselets[FALSE_TOKEN] = parser_parse_boolean;
  p->prefix_parselets[LPAREN_TOKEN] = parser_parse_grouped_expression;
  p->prefix_parselets[IF_TOKEN] = parser_parse_if_expression;

  p->infix_parselets = calloc(RETURN_TOKEN + 1, sizeof(infix_parse_fn));
  assert(p->infix_parselets);
  p->infix_parselets[PLUS_TOKEN] = parser_parse_infix;
  p->infix_parselets[MINUS_TOKEN] = parser_parse_infix;
  p->infix_parselets[SLASH_TOKEN] = parser_parse_infix;
  p->infix_parselets[ASTERISK_TOKEN] = parser_parse_infix;
  p->infix_parselets[EQ_TOKEN] = parser_parse_infix;
  p->infix_parselets[NOT_EQ_TOKEN] = parser_parse_infix;
  p->infix_parselets[LT_TOKEN] = parser_parse_infix;
  p->infix_parselets[GT_TOKEN] = parser_parse_infix;

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

    free(p->prefix_parselets);
    free(p->infix_parselets);
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
  const char *expected_token_str = token_to_str(token_type);
  const char *actual_token_str = token_to_str(parser->peek_token->type);
  sprintf(error, "expected next token to be %s, got %s instead",
          expected_token_str, actual_token_str);
  parser_append_error(parser, error);
}

program_t *parser_parse_program(parser_t *parser) {

  program_t *program = program_new();

  while (parser->cur_token->type != EOF_TOKEN) {
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

  if (cur_token_type == LET_TOKEN) {
    let_statement_t *let_statement = parser_parse_let_statement(parser);
    return statement_new((void *)let_statement, LET_STATEMENT);
  } else if (cur_token_type == RETURN_TOKEN) {
    return statement_new((void *)parser_parse_return_statement(parser),
                         RETURN_STATEMENT);
  } else {
    expression_statement_t *expression_statement =
        parser_parse_expression_statement(parser);
    return statement_new(expression_statement, EXPRESSION_STATEMENT);
  }
  /* Never reaches here */
}

let_statement_t *parser_parse_let_statement(parser_t *parser) {
  assert(parser);

  token_t *let_token = parser->cur_token;

  if (!parser_expect_peek(parser, IDENT_TOKEN)) {
    return NULL;
  }

  identifier_t *name = identifier_new(parser->cur_token);

  if (!parser_expect_peek(parser, ASSIGN_TOKEN)) {
    return NULL;
  }

  /*
   * TODO: We're skipping the expressions until we encounter a semicolon;
   */

  expression_t *value = NULL;

  while (!parser_cur_token_is(parser, SEMICOLON_TOKEN)) {
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

  while (!parser_cur_token_is(parser, SEMICOLON_TOKEN)) {
    token_destroy(&parser->cur_token);
    parser_next_token(parser);
  }

  return return_statement_new(return_token, return_value);
}

expression_statement_t *parser_parse_expression_statement(parser_t *parser) {
  assert(parser);
  token_t *token = parser->cur_token;
  /* GIVE PRECEDENCE */
  expression_t *expression = parser_parse_expression(parser, LOWEST_PRECEDENCE);
  if (parser_peek_token_is(parser, SEMICOLON_TOKEN)) {
    /* token_destroy(&parser->cur_token); */
    parser_next_token(parser);
  }
  /* token_destroy(&parser->cur_token); */
  expression_statement_t *expression_statement =
      expression_statement_new(token, expression);
  return expression_statement;
}

block_statement_t *parser_parse_block_statement(parser_t *parser) {
  assert(parser);
  token_t *block_token = parser->cur_token;
  statement_t **statements = NULL;
  size_t statements_len = 0;
  parser_next_token(parser);

  while (!parser_cur_token_is(parser, RBRACE_TOKEN) && !parser_cur_token_is(parser, EOF_TOKEN)) {
    statement_t *statement = parser_parse_statement(parser);
    if (statement != NULL) {
      if (statements == NULL) {
        statements = malloc(sizeof(statement_t *));
        statements[0] = statement;
        statements_len += 1;
      } else {
        statements =
          realloc(statements, sizeof(statement_t *) * (statements_len + 1));
        if (statements == NULL) {
          /*
           * TODO: Better error handling needed
           */
          puts("Cannot allocate for statements");
          exit(1);
        }
        /* program->statements = statements; */
        statements[statements_len] = statement;
        statements_len += 1;
      }
    }
    parser_next_token(parser);
  }

  block_statement_t *block_statement = block_statement_new(block_token, statements, statements_len);
  if (parser_cur_token_is(parser, RBRACE_TOKEN) || parser_cur_token_is(parser, EOF_TOKEN)) {
    token_destroy(&parser->cur_token);
    parser_next_token(parser);
  }
  return block_statement;
}

expression_t *parser_parse_expression(parser_t *parser, PRECEDENCE precedence) {
  token_t *token = parser->cur_token;
  prefix_parse_fn prefix = parser->prefix_parselets[token->type];

  if (prefix == NULL) {
    char *err_str = NULL;
    asprintf(&err_str, "Error: %s(%d)\n", token->literal, token->type);
    parser_append_error(parser, err_str);
    free(err_str);
    /* printf("Error: %s(%d)\n", token->literal, token->type); */
    /* assert("Couldn't find a prefix for token"); */
    return NULL;
  }

  expression_t *left_expression = prefix(parser, token, precedence);

  while (!parser_peek_token_is(parser, SEMICOLON_TOKEN) &&
         precedence < parser_peek_precedence(parser)) {
    parser_next_token(parser);

    token = parser->cur_token;

    infix_parse_fn infix = parser->infix_parselets[token->type];

    if (infix == NULL) {
      return left_expression;
    }

    left_expression = infix(parser, token, precedence, left_expression);
  }

  /* If we have already reached EOF TOKEN, just destroy it and return */
  if (parser->cur_token != NULL && parser_cur_token_is(parser, EOF_TOKEN)) {
    token_destroy(&parser->cur_token);
    return left_expression;
  }

  /* Should check for semicolon too? */
  if (parser_peek_token_is(parser, EOF_TOKEN)) {
    parser_next_token(parser);
    token_destroy(&parser->cur_token);
  }

  return left_expression;
}

expression_t *parser_parse_identifier(parser_t *parser, token_t *token,
                                      PRECEDENCE precedence) {
  assert(token);
  identifier_t *identifier = identifier_new(token);
  expression_t *expression = expression_new(IDENT_EXP, identifier);
  return expression;
}

expression_t *parser_parse_integer(parser_t *parser, token_t *token,
                                   PRECEDENCE precedence) {
  assert(token);
  integer_t *integer = integer_new(token);
  expression_t *expression = expression_new(INT_EXP, integer);
  return expression;
}

expression_t *parser_parse_boolean(parser_t *parser, token_t *token,
                                   PRECEDENCE precedence) {
  assert(token);
  boolean_t *boolean = boolean_new(token);
  expression_t *expression = expression_new(BOOLEAN_EXP, boolean);
  return expression;
}

expression_t *parser_parse_grouped_expression(parser_t *parser, token_t *token,
                                              PRECEDENCE precedence) {
  assert(token);
  /* Destroy LPAREN TOKEN */
  token_destroy(&parser->cur_token);
  parser_next_token(parser);

  expression_t *expression = parser_parse_expression(parser, LOWEST_PRECEDENCE);

  if (!parser_expect_peek(parser, RPAREN_TOKEN)) {
    return NULL;
  }

  /* Destroy RPAREN token */
  token_destroy(&parser->cur_token);

  return expression;
}

expression_t *parser_parse_if_expression(parser_t *parser, token_t *token,
                                         PRECEDENCE precedence) {
  assert(token);
  token_t *if_token = token;

  if (!parser_expect_peek(parser, LPAREN_TOKEN)) {
    assert("Expected LPAREN");
    return NULL;
  }
  /* Destroy LPAREN */
  token_destroy(&parser->cur_token);
  parser_next_token(parser);

  expression_t *condition = parser_parse_expression(parser, LOWEST_PRECEDENCE);

  if (!parser_expect_peek(parser, RPAREN_TOKEN)) {
    assert("Expected RPAREN");
    return NULL;
  }
  /* Destroy RPAREN */
  token_destroy(&parser->cur_token);

  if (!parser_expect_peek(parser, LBRACE_TOKEN)) {
    assert("Expected LBRACE");
    return NULL;
  }

  block_statement_t *consequence = parser_parse_block_statement(parser);
  if_exp_t *if_exp = if_exp_new(if_token, condition, consequence, NULL);  
  return expression_new(IF_EXP, if_exp);
}

expression_t *parser_parse_prefix(parser_t *parser, token_t *token,
                                  PRECEDENCE precedence) {
  assert(parser);
  assert(token);
  token_t *operator= token;

  parser_next_token(parser);

  expression_t *operand = parser_parse_expression(parser, PREFIX_PRECEDENCE);
  return expression_new(PREFIX_EXP, prefix_new(operator, operand));
}

expression_t *parser_parse_infix(parser_t *parser, token_t *token,
                                 PRECEDENCE precedence, expression_t *left) {
  assert(parser);
  assert(token);
  assert(left);

  token_t *operator= token;
  PRECEDENCE cur_precedence = token_get_precedence(operator);

  parser_next_token(parser);

  expression_t *right = parser_parse_expression(parser, cur_precedence);
  infix_t *infix = infix_new(operator, left, right);
  expression_t *expression = expression_new(INFIX_EXP, infix);
  return expression;
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

PRECEDENCE token_get_precedence(token_t *token) {
  assert(token);
  switch (token->type) {
  case PLUS_TOKEN:
  case MINUS_TOKEN:
    return SUM_PRECEDENCE;
  case SLASH_TOKEN:
  case ASTERISK_TOKEN:
    return PRODUCT_PRECEDENCE;
  case EQ_TOKEN:
  case NOT_EQ_TOKEN:
    return EQUALS_PRECEDENCE;
  case LT_TOKEN:
  case GT_TOKEN:
    return LESSGREATER_PRECEDENCE;
  default:
    return LOWEST_PRECEDENCE;
  }
}

PRECEDENCE parser_cur_precedence(parser_t *parser) {
  assert(parser);
  return token_get_precedence(parser->cur_token);
}

PRECEDENCE parser_peek_precedence(parser_t *parser) {
  assert(parser);
  return token_get_precedence(parser->peek_token);
}
