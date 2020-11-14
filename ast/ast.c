#include "ast.h"

char *get_token_literal(token_t *tok) { return strdup(tok->literal); }

statement_t *statement_new(void *statement, STATEMENT_TYPE st) {
  statement_t *s = malloc(sizeof(statement_t));

  switch (st) {
  case LET_STATEMENT:
    s->type = LET_STATEMENT;
    s->statement.let_statement = (let_statement_t *)statement;
    return s;
  case RETURN_STATEMENT:
    s->type = RETURN_STATEMENT;
    s->statement.return_statement = (return_statement_t *)statement;
    return s;
  case EXPRESSION_STATEMENT:
    s->type = EXPRESSION_STATEMENT;
    s->statement.expression_statement = (expression_statement_t *)statement;
    return s;
  default:
    return NULL;
  }
}

void statement_destroy(statement_t **s_p, STATEMENT_TYPE st) {
  assert(s_p);
  if (*s_p) {
    statement_t *statement = *s_p;
    switch (statement->type) {
    case LET_STATEMENT:
      let_statement_destroy(&statement->statement.let_statement);
      break;
    case RETURN_STATEMENT:
      return_statement_destroy(&statement->statement.return_statement);
    case EXPRESSION_STATEMENT:
      expression_statement_destroy(&statement->statement.expression_statement);
    }
    free(statement);
    *s_p = NULL;
  }
}

char *statement_to_string(statement_t *statement) {
  assert(statement);
  switch (statement->type) {
  case LET_STATEMENT:
    return let_statement_to_string(
        (let_statement_t *)statement->statement.let_statement);
  case RETURN_STATEMENT:
    return return_statement_to_string(
        (return_statement_t *)statement->statement.return_statement);
  case EXPRESSION_STATEMENT:
    return expression_statement_to_string(
        (expression_statement_t *)statement->statement.expression_statement);
  default:
    return NULL;
  }
}

expression_t *expression_new(EXPRESSION_TYPE e_type, void *expression) {
  assert(expression);
  expression_t *exp = malloc(sizeof(expression_t));
  assert(exp);
  exp->type = e_type;
  switch (e_type) {
  case IDENT_EXP:
    exp->expression.identifier = (identifier_t *)expression;
    break;
  case INT_EXP:
    exp->expression.integer = (integer_t *)expression;
    break;
  case BOOLEAN_EXP:
    exp->expression.boolean = (boolean_t *)expression;
    break;
  case PREFIX_EXP:
    exp->expression.prefix = (prefix_t *)expression;
    break;
  case INFIX_EXP:
    exp->expression.infix = (infix_t *)expression;
    break;
  default:
    assert("Invalid expression");
  }
  return exp;
}

void expression_destroy(expression_t **e_p) {
  assert(e_p);
  if (*e_p) {
    expression_t *expression = *e_p;
    switch (expression->type) {
    case IDENT_EXP:
      identifier_destroy(&expression->expression.identifier);
      break;
    case INT_EXP:
      integer_destroy(&expression->expression.integer);
      break;
    case BOOLEAN_EXP:
      boolean_destroy(&expression->expression.boolean);
      break;
    case PREFIX_EXP:
      prefix_destroy(&expression->expression.prefix);
      break;
    case INFIX_EXP:
      infix_destroy(&expression->expression.infix);
      break;
    default:
      assert("Invalid expression");
    }
    free(expression);
    *e_p = NULL;
  }
}

char *expression_to_string(expression_t *expression) {
  assert(expression);

  char *expression_str = NULL;
  switch (expression->type) {
  case INT_EXP:
    return integer_to_string(expression->expression.integer);
  case BOOLEAN_EXP:
    return boolean_to_string(expression->expression.boolean);
  case IDENT_EXP:
    return identifier_to_string(expression->expression.identifier);
  case PREFIX_EXP:
    return prefix_to_string(expression->expression.prefix);
  case INFIX_EXP:
    return infix_to_string(expression->expression.infix);
  default:
    puts("Error: Unknown expression");
    assert(false);
    return NULL;
  }
}

identifier_t *identifier_new(token_t *token) {
  assert(token);
  assert(token->literal);
  identifier_t *identifier = malloc(sizeof(identifier_t));
  identifier->token = token;
  identifier->value = strdup(token->literal);
  return identifier;
}

void identifier_destroy(identifier_t **i_p) {
  assert(i_p);
  if (*i_p) {
    identifier_t *i = *i_p;
    token_destroy(&i->token);
    assert(i->value);
    free(i->value);
    free(i);
    *i_p = NULL;
  }
}

char *identifier_to_string(identifier_t *identifier) {
  assert(identifier);
  return strdup(identifier->value);
}

integer_t *integer_new(token_t *token) {
  assert(token);
  integer_t *integer = malloc(sizeof(integer_t));
  integer->token = token;

  int32_t value;
  const char *errstr = NULL;
  value = strtonum(token->literal, 1, INT32_MAX, &errstr);

  if (errstr != NULL) {
    printf("Invalid number: Unable to parse number %s\n", errstr);
    assert(false);
  }

  assert(((INT32_MIN <= value) && (value <= INT32_MAX)) &&
         "Invalid number: Number not in range.");

  integer->value = value;
  return integer;
}

void integer_destroy(integer_t **i_p) {
  assert(i_p);
  if (*i_p) {
    integer_t *integer = *i_p;
    token_destroy(&integer->token);
    free(integer);
    *i_p = NULL;
  }
}

char *integer_to_string(integer_t *integer) {
  assert(integer);
  char *str = NULL;
  asprintf(&str, "%" PRId32, integer->value);
  return str;
}

boolean_t *boolean_new(token_t *token) {
  assert(token);
  boolean_t *boolean = malloc(sizeof(boolean_t));
  boolean->token = token;
  boolean->value = token->type == TRUE_TOKEN;
}

void boolean_destroy(boolean_t **b_p) {
  assert(b_p);
  if (*b_p) {
    boolean_t *boolean = *b_p;
    token_destroy(&boolean->token);
    free(boolean);
    *b_p = NULL;
  }
}

char *boolean_to_string(boolean_t *b) {
  assert(b);
  return strdup(b->token->type == TRUE_TOKEN ? "true" : "false" );
}

prefix_t *prefix_new(token_t *operator, expression_t * operand) {
  assert(operator);
  assert(operand);
  prefix_t *prefix = malloc(sizeof(prefix_t));
  assert(prefix);
  prefix->operator= operator;
  prefix->operand = operand;
  return prefix;
}

void prefix_destroy(prefix_t **p_p) {
  assert(p_p);
  if (*p_p) {
    prefix_t *prefix = *p_p;
    expression_destroy(&prefix->operand);
    token_destroy(&prefix->operator);
    free(prefix);
    *p_p = NULL;
  }
}

char *prefix_to_string(prefix_t *prefix) {
  assert(prefix);
  char *str = NULL;
  char *expression_str = expression_to_string(prefix->operand);
  asprintf(&str, "(%s%s)", prefix->operator->literal, expression_str);
  free(expression_str);
  return str;
}

infix_t *infix_new(token_t *operator, expression_t * left,
                   expression_t * right) {
  assert(operator);
  assert(left);
  assert(right);
  infix_t *infix = malloc(sizeof(infix_t));
  assert(infix);
  infix->operator= operator;
  infix->left = left;
  infix->right = right;
  return infix;
}

void infix_destroy(infix_t **i_p) {
  assert(i_p);
  if (*i_p) {
    infix_t *infix = *i_p;
    token_destroy(&infix->operator);
    expression_destroy(&infix->left);
    expression_destroy(&infix->right);
    free(infix);
    *i_p = NULL;
  }
}

char *infix_to_string(infix_t *infix) {
  char *str = NULL;
  char *left_exp_str = expression_to_string(infix->left);
  char *right_exp_str = expression_to_string(infix->right);
  asprintf(&str, "(%s %s %s)", left_exp_str, infix->operator->literal,
           right_exp_str);
  free(left_exp_str);
  free(right_exp_str);
  return str;
}

let_statement_t *let_statement_new(token_t *token, identifier_t *name,
                                   expression_t *value) {
  assert(token);
  assert(name);
  /* TODO: ASSERT VALUE */
  /* assert(value); */
  let_statement_t *let = malloc(sizeof(let_statement_t));
  let->token = token;
  let->name = name;
  let->value = value;
  return let;
}

void let_statement_destroy(let_statement_t **l_p) {
  assert(l_p);
  if (*l_p) {
    let_statement_t *l = *l_p;
    assert(l);

    token_destroy(&l->token);
    identifier_destroy(&l->name);
    /* TODO: destroy expression */
    /* if (l->value != NULL) expression_destroy(&l->value); */
    free(l);
    *l_p = NULL;
  }
}

char *let_statement_to_string(let_statement_t *let_statement) {
  assert(let_statement);
  char *buffer = NULL;
  char *value_str = expression_to_string(let_statement->value);
  asprintf(&buffer, "let %s = %s;", let_statement->name->value, value_str);
  free(value_str);
  return buffer;
}

return_statement_t *return_statement_new(token_t *token,
                                         expression_t *return_value) {
  assert(token);
  /* TODO: ASSERT return_value */
  /* assert(return_value); */

  return_statement_t *return_statement = malloc(sizeof(return_statement_t));
  assert(return_statement);
  return_statement->token = token;
  return_statement->return_value = return_value;
  return return_statement;
}

void return_statement_destroy(return_statement_t **r_p) {
  assert(r_p);
  if (*r_p) {
    return_statement_t *return_statement = *r_p;
    assert(return_statement);

    token_destroy(&return_statement->token);
    /* TODO: destroy expression */
    /* if (return_statement->value != NULL)
     * expression_destroy(&return_statement->value); */
    free(return_statement);
    *r_p = NULL;
  }
}

char *return_statement_to_string(return_statement_t *return_statement) {
  assert(return_statement);
  char *buffer = NULL;
  char *return_value_str = expression_to_string(return_statement->return_value);
  asprintf(&buffer, "return %s;", return_value_str);
  free(return_value_str);
  return buffer;
}

expression_statement_t *expression_statement_new(token_t *token,
                                                 expression_t *expression) {
  assert(token);
  assert(expression);
  expression_statement_t *est = malloc(sizeof(expression_statement_t));
  assert(est);
  est->token = token;
  est->expression = expression;
  return est;
}

void expression_statement_destroy(expression_statement_t **e_p) {
  assert(e_p);
  if (*e_p) {
    expression_statement_t *expression = *e_p;
    expression_destroy(&expression->expression);
    free(expression);
    *e_p = NULL;
  }
}

char *
expression_statement_to_string(expression_statement_t *expression_statement) {
  assert(expression_statement);
  return expression_to_string(expression_statement->expression);
}

program_t *program_new() {
  program_t *p = malloc(sizeof(program_t));
  p->statements = NULL;
  p->len = 0;
  return p;
}

void program_destroy(program_t **p_p) {
  assert(p_p);
  if (*p_p) {
    program_t *p = *p_p;
    assert(p);
    for (int i = 0; i < p->len; i++) {
      statement_destroy(&p->statements[i], p->statements[i]->type);
    }
    free(p->statements);
    free(p);
    *p_p = NULL;
  }
}

void program_append_statement(program_t *program, statement_t *statement) {
  assert(program);
  assert(statement);

  if (program->statements == NULL) {

    program->statements = malloc(sizeof(statement_t *));
    program->statements[0] = statement;
    program->len += 1;

    return;
  }

  statement_t **statements =
      realloc(program->statements, sizeof(statement_t *) * (program->len + 1));
  if (statements == NULL) {
    /*
     * TODO: Better error handling needed
     */
    puts("Cannot allocate for statements");
    exit(1);
  }
  program->statements = statements;
  program->statements[program->len] = statement;
  program->len += 1;
}

char *program_to_string(program_t *program) {

  assert(program);

  char *program_str = NULL;
  statement_t **statements = program->statements;
  size_t statements_len = program->len;
  int i = 0;
  unsigned int char_count = 0;

  while (i < statements_len) {
    char *statement_str = statement_to_string(statements[i]);
    program_str = reallocarray(program_str,
                               (program_str == NULL ? 0 : strlen(program_str)) +
                                   strlen(statement_str) + 1,
                               sizeof(char));
    char_count += snprintf(program_str + char_count, strlen(statement_str) + 1,
                           "%s", statement_str);
    free(statement_str);
    i++;
  }

  return program_str;
}
