#include "ast.h"

char *get_token_literal(token_t *tok) { return strdup(tok->literal); }

statement_t *statement_new(void *statement, STATEMENT_TYPE st) {
  statement_t *s = malloc(sizeof(statement_t));

  switch (st) {
  case LET_STATEMENT:
    s->type = LET_STATEMENT;
    s->statement.let_statement = (let_statement_t *)statement;
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
    }
    free(statement);
    *s_p = NULL;
  }
}

identifier_t *identifier_new(token_t *token, char *value) {
  assert(token);
  assert(value);
  identifier_t *identifier = malloc(sizeof(identifier_t));
  identifier->token = token;
  identifier->value = strdup(value);
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
