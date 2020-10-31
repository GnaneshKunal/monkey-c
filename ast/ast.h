#ifndef AST_H
#define AST_H

#include "../token/token.h"
#include "../utils/utils.h"

typedef struct _expression_t expression_t;
typedef struct _statement_t statement_t;

typedef enum {
  IDENT_EXP,
} EXPRESSION_TYPE;

typedef struct _identifier_t {
  token_t *token;
  char *value;
} identifier_t;

identifier_t *identifier_new(token_t *token, char *value);
void identifier_destroy(identifier_t **i_p);

/* forward declaration at the top */
struct _exp_t {
  EXPRESSION_TYPE type;
  union {
    /* expressions */
  };
};

typedef enum {
  LET_STATEMENT,
} STATEMENT_TYPE;

typedef struct _let_statement_t {
  token_t *token;
  identifier_t *name;
  expression_t *value;
} let_statement_t;

let_statement_t *let_statement_new(token_t *token, identifier_t *name,
                                   expression_t *value);
void let_statement_destroy(let_statement_t **l_p);

/* forward declaration at the top */
struct _statement_t {
  STATEMENT_TYPE type;
  union {
    let_statement_t *let_statement;
  } statement;
};
statement_t *statement_new(void *statement, STATEMENT_TYPE st);
void statement_destroy(statement_t **s_p, STATEMENT_TYPE st);

struct _program_t {
  statement_t **statements;
  size_t len;
};

typedef struct _program_t program_t;
program_t *program_new();
void program_destroy(program_t **p_p);
void program_append_statement(program_t *program, statement_t *statement);

char *get_token_literal(token_t *tok);

#endif
