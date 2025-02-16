#include "ast.h"
#include "config.h"

statement_t *statement_new(void *statement, STATEMENT_TYPE st) {
  statement_t *s = malloc(sizeof(statement_t));

  switch (st) {
  case LET_STATEMENT:
    s->type = LET_STATEMENT;
    s->let_statement = (let_statement_t *)statement;
    return s;
  case RETURN_STATEMENT:
    s->type = RETURN_STATEMENT;
    s->return_statement = (return_statement_t *)statement;
    return s;
  case EXPRESSION_STATEMENT:
    s->type = EXPRESSION_STATEMENT;
    s->expression_statement = (expression_statement_t *)statement;
    return s;
  case BLOCK_STATEMENT:
    s->type = BLOCK_STATEMENT;
    s->block_statement = (block_statement_t *)statement;
  default:
    return NULL;
  }
}

void statement_destroy(statement_t **s_p) {
  assert(s_p);
  if (*s_p) {
    statement_t *statement = *s_p;
    switch (statement->type) {
    case LET_STATEMENT:
      let_statement_destroy(&statement->let_statement);
      break;
    case RETURN_STATEMENT:
      return_statement_destroy(&statement->return_statement);
    case EXPRESSION_STATEMENT:
      expression_statement_destroy(&statement->expression_statement);
    case BLOCK_STATEMENT:
      block_statement_destroy(&statement->block_statement);
    }
    free(statement);
    *s_p = NULL;
  }
}

char *statement_to_string(statement_t *statement) {
  assert(statement);
  switch (statement->type) {
  case LET_STATEMENT:
    return let_statement_to_string(statement->let_statement);
  case RETURN_STATEMENT:
    return return_statement_to_string(statement->return_statement);
  case EXPRESSION_STATEMENT:
    return expression_statement_to_string(statement->expression_statement);
  case BLOCK_STATEMENT:
    return block_statement_to_string(statement->block_statement);
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
    exp->identifier = (identifier_t *)expression;
    break;
  case INT_EXP:
    exp->integer = (integer_t *)expression;
    break;
  case BOOLEAN_EXP:
    exp->boolean = (boolean_t *)expression;
    break;
  case PREFIX_EXP:
    exp->prefix = (prefix_t *)expression;
    break;
  case INFIX_EXP:
    exp->infix = (infix_t *)expression;
    break;
  case IF_EXP:
    exp->if_exp = (if_exp_t *)expression;
    break;
  case FN_EXP:
    exp->fn = (fn_t *)expression;
    break;
  case CALL_EXP:
    exp->call_exp = (call_exp_t *)expression;
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
      identifier_destroy(&expression->identifier);
      break;
    case INT_EXP:
      integer_destroy(&expression->integer);
      break;
    case BOOLEAN_EXP:
      boolean_destroy(&expression->boolean);
      break;
    case PREFIX_EXP:
      prefix_destroy(&expression->prefix);
      break;
    case INFIX_EXP:
      infix_destroy(&expression->infix);
      break;
    case IF_EXP:
      if_exp_destroy(&expression->if_exp);
      break;
    case FN_EXP:
      fn_destroy(&expression->fn);
      break;
    case CALL_EXP:
      call_exp_destroy(&expression->call_exp);
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
    return integer_to_string(expression->integer);
  case BOOLEAN_EXP:
    return boolean_to_string(expression->boolean);
  case IDENT_EXP:
    return identifier_to_string(expression->identifier);
  case PREFIX_EXP:
    return prefix_to_string(expression->prefix);
  case INFIX_EXP:
    return infix_to_string(expression->infix);
  case IF_EXP:
    return if_exp_to_string(expression->if_exp);
  case FN_EXP:
    return fn_to_string(expression->fn);
  case CALL_EXP:
    return call_exp_to_string(expression->call_exp);
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
#ifdef HAVE_LIBBSD
  const char *errstr = NULL;
  value = strtonum(token->literal, 1, INT32_MAX, &errstr);

  if (errstr != NULL) {
    printf("Invalid number: Unable to parse number %s\n", errstr);
    assert(false);
  }
#else
  if (sscanf(token->literal, "%" SCNd32, &value) != 1) {
    printf("Invalid number: Unable to parse number %s\n", token->literal);
    assert(false);
  }
#endif

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
  return boolean;
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
  return strdup(b->token->type == TRUE_TOKEN ? "true" : "false");
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

if_exp_t *if_exp_new(token_t *token, expression_t *condition,
                     block_statement_t *consequence,
                     block_statement_t *alternative) {
  assert(token);
  assert(condition);
  assert(consequence);
  /* else part is optional */
  /* assert(alternative); */
  if_exp_t *if_exp = malloc(sizeof(if_exp_t));
  if_exp->token = token;
  if_exp->condition = condition;
  if_exp->consequence = consequence;
  if_exp->alternative = alternative;
  return if_exp;
}

void if_exp_destroy(if_exp_t **i_p) {
  assert(i_p);
  if (*i_p) {
    if_exp_t *if_exp = *i_p;
    token_destroy(&if_exp->token);
    expression_destroy(&if_exp->condition);
    block_statement_destroy(&if_exp->consequence);
    block_statement_destroy(&if_exp->alternative);
    free(if_exp);
    *i_p = NULL;
  }
}

char *if_exp_to_string(if_exp_t *if_exp) {
  assert(if_exp);
  char *str = NULL;
  char *condition_str = expression_to_string(if_exp->condition);
  char *consequence_str = block_statement_to_string(if_exp->consequence);

  asprintf(&str, "if %s %s", condition_str, consequence_str);
  free(condition_str);
  free(consequence_str);

  char *final_str = NULL;

  if (if_exp->alternative != NULL) {
    char *alternative_str = block_statement_to_string(if_exp->alternative);
    asprintf(&final_str, "%s else %s", str, alternative_str);
    free(alternative_str);
  }

  if (final_str != NULL) {
    free(str);
    return final_str;
  }

  return str;
}

param_t *param_new(void) {
  param_t *params = malloc(sizeof(param_t));
  params->parameters = NULL;
  params->len = 0;
  return params;
}

void param_append(param_t *params, identifier_t *identifier) {
  assert(params);
  assert(identifier);
  params->parameters =
      realloc(params->parameters, (params->len + 1) * sizeof(identifier_t *));
  assert(params->parameters);
  params->parameters[params->len++] = identifier;
}

void param_destroy(param_t **p_p) {
  assert(p_p);
  if (*p_p) {
    param_t *params = *p_p;
    for (int i = 0; i < params->len; i++) {
      identifier_destroy(&params->parameters[i]);
    }
    free(params->parameters);
    free(params);
    *p_p = NULL;
  }
}

char *param_to_string(param_t *params) {
  assert(params);
  char *str = NULL;
  asprintf(&str, "(");
  for (int i = 0; i < params->len; i++) {
    char *tmp_str = NULL;
    char *i_str = identifier_to_string(params->parameters[i]);
    if (i == 0) {
      asprintf(&tmp_str, "%s%s", str, i_str);
    } else {
      asprintf(&tmp_str, "%s, %s", str, i_str);
    }
    free(i_str);
    free(str);
    str = tmp_str;
  }
  char *str2 = NULL;
  asprintf(&str2, "%s)", str);
  free(str);
  return str2;
}

param_exp_t *param_exp_new(void) {
  param_exp_t *param_exps = malloc(sizeof(param_exp_t));
  param_exps->expressions = NULL;
  param_exps->len = 0;
  return param_exps;
}

void param_exp_append(param_exp_t *param_exps, expression_t *expression) {
  assert(param_exps);
  assert(expression);
  param_exps->expressions = realloc(
      param_exps->expressions, (param_exps->len + 1) * sizeof(expression_t *));
  assert(param_exps->expressions);
  param_exps->expressions[param_exps->len++] = expression;
}

void param_exp_destroy(param_exp_t **p_p) {
  assert(p_p);
  if (*p_p) {
    param_exp_t *param_exps = *p_p;
    for (int i = 0; i < param_exps->len; i++) {
      expression_destroy(&param_exps->expressions[i]);
    }
    free(param_exps->expressions);
    free(param_exps);
    *p_p = NULL;
  }
}

char *param_exp_to_string(param_exp_t *param_exps) {
  assert(param_exps);
  char *str = NULL;
  asprintf(&str, "(");
  for (int i = 0; i < param_exps->len; i++) {
    char *tmp_str = NULL;
    char *e_str = expression_to_string(param_exps->expressions[i]);
    if (i == 0) {
      asprintf(&tmp_str, "%s%s", str, e_str);
    } else {
      asprintf(&tmp_str, "%s, %s", str, e_str);
    }
    free(e_str);
    free(str);
    str = tmp_str;
  }
  char *str2 = NULL;
  asprintf(&str2, "%s)", str);
  free(str);
  return str2;
}

fn_t *fn_new(token_t *token, param_t *params, block_statement_t *body) {
  assert(token);
  /* Zero params */
  /* assert(params); */
  assert(body);

  fn_t *function_literal = malloc(sizeof(fn_t));
  function_literal->token = token;
  function_literal->params = params;
  function_literal->body = body;

  return function_literal;
}

void fn_destroy(fn_t **f_p) {
  assert(f_p);
  if (*f_p) {
    fn_t *function_literal = *f_p;
    token_destroy(&function_literal->token);
    param_destroy(&function_literal->params);
    block_statement_destroy(&function_literal->body);
    free(function_literal);
    *f_p = NULL;
    return;
  }
}

char *fn_to_string(fn_t *function_literal) {
  assert(function_literal);
  char *str = NULL;
  char *params_str = param_to_string(function_literal->params);
  char *block_statement_str = block_statement_to_string(function_literal->body);
  asprintf(&str, "fn%s %s", params_str, block_statement_str);
  free(params_str);
  free(block_statement_str);
  return str;
}

call_exp_t *call_exp_new(token_t *token, param_exp_t *param_exps,
                         expression_t *exp) {
  assert(token);
  assert(param_exps);
  assert(exp);
  call_exp_t *call_exp = malloc(sizeof(call_exp_t));
  call_exp->token = token;
  call_exp->call_exp = exp;
  call_exp->param_exps = param_exps;
  return call_exp;
}

void call_exp_destroy(call_exp_t **c_p) {
  assert(c_p);
  if (*c_p) {
    call_exp_t *call_exp = *c_p;
    token_destroy(&call_exp->token);
    expression_destroy(&call_exp->call_exp);
    param_exp_destroy(&call_exp->param_exps);
    free(call_exp);
    *c_p = NULL;
  }
}

char *call_exp_to_string(call_exp_t *call_exp) {
  assert(call_exp);
  char *str = NULL;
  char *params_str = param_exp_to_string(call_exp->param_exps);
  char *expression_str = expression_to_string(call_exp->call_exp);
  asprintf(&str, "%s%s", expression_str, params_str);
  free(params_str);
  free(expression_str);
  return str;
}

let_statement_t *let_statement_new(token_t *token, identifier_t *name,
                                   expression_t *value) {
  assert(token);
  assert(name);
  assert(value);
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
    if (l->value != NULL)
      expression_destroy(&l->value);
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
  assert(return_value);

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
    if (return_statement->return_value != NULL)
      expression_destroy(&return_statement->return_value);
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

block_statement_t *block_statement_new(token_t *token, statement_t **statements,
                                       size_t statements_len) {
  assert(token);
  /* Empty block statements */
  /* assert(statements); */
  /* assert(statements_len > 0); */
  block_statement_t *block_statement = malloc(sizeof(block_statement_t));
  block_statement->token = token;
  block_statement->statements = statements;
  block_statement->statements_len = statements_len;
  return block_statement;
}

void block_statement_destroy(block_statement_t **b_p) {
  assert(b_p);
  if (*b_p) {
    block_statement_t *block_statement = *b_p;
    assert(block_statement);
    token_destroy(&block_statement->token);
    size_t statements_len = block_statement->statements_len;
    for (int i = 0; i < statements_len; i++) {
      statement_destroy(&block_statement->statements[i]);
    }
    free(block_statement->statements);
    free(block_statement);
    *b_p = NULL;
  }
}

char *block_statement_to_string(block_statement_t *block_statement) {
  assert(block_statement);
  char *str = NULL;
  asprintf(&str, "{");
  for (int i = 0; i < block_statement->statements_len; i++) {
    char *temp_str = NULL;
    char *statement_str = statement_to_string(block_statement->statements[i]);
    asprintf(&temp_str, "%s %s", str, statement_str);
    free(statement_str);
    free(str);
    str = temp_str;
  }
  char *temp_str = NULL;
  asprintf(&temp_str, "%s }", str);
  free(str);
  str = temp_str;
  return str;
}

program_t *program_new(void) {
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
      statement_destroy(&p->statements[i]);
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
#ifdef HAVE_REALLOCARRAY
    program_str = reallocarray(program_str,
                               (program_str == NULL ? 0 : strlen(program_str)) +
                                   strlen(statement_str) + 1,
                               sizeof(char));
#else
    program_str =
        realloc(program_str, ((program_str == NULL ? 0 : strlen(program_str)) +
                              strlen(statement_str) + 1) *
                                 sizeof(char));
#endif
    char_count += snprintf(program_str + char_count, strlen(statement_str) + 1,
                           "%s", statement_str);
    free(statement_str);
    i++;
  }

  return program_str;
}

const char *expression_type_to_str(EXPRESSION_TYPE et) {
  switch (et) {
  case IDENT_EXP:
    return "IDENT_EXP";
  case INT_EXP:
    return "INT_EXP";
  case BOOLEAN_EXP:
    return "BOOLEAN_EXP";
  case PREFIX_EXP:
    return "PREFIX_EXP";
  case INFIX_EXP:
    return "INFIX_EXP";
  case FN_EXP:
    return "FN_EXP";
  case CALL_EXP:
    return "CALL_EXP";
  default:
    assert("Unknown expression");
    return NULL;
  }
}

const char *statement_type_to_str(STATEMENT_TYPE st) {
  switch (st) {
  case LET_STATEMENT:
    return "LET_STATEMENT";
  case RETURN_STATEMENT:
    return "RETURN_STATEMENT";
  case EXPRESSION_STATEMENT:
    return "EXPRESSION_STATEMENT";
  default:
    assert("Unknown statement");
    return NULL;
  }
}
