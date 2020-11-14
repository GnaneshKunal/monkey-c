#ifndef AST_H
#define AST_H

#include "../token/token.h"
#include "../utils/utils.h"

typedef struct _expression_t expression_t;
typedef struct _statement_t statement_t;

typedef enum {
  IDENT_EXP,
  INT_EXP,
  BOOLEAN_EXP,
  PREFIX_EXP,
  INFIX_EXP,
} EXPRESSION_TYPE;

typedef struct _identifier_t {
  token_t *token;
  char *value;
} identifier_t;

identifier_t *identifier_new(token_t *token);
void identifier_destroy(identifier_t **i_p);
char *identifier_to_string(identifier_t *identifier);

typedef struct _integer_t {
  token_t *token;
  int32_t value;
} integer_t;

integer_t *integer_new(token_t *token);
void integer_destroy(integer_t **i_p);
char *integer_to_string(integer_t *integer);

typedef struct _boolean_t {
  token_t *token;
  bool value;
} boolean_t;

boolean_t *boolean_new(token_t *token);
void boolean_destroy(boolean_t **b_p);
char *boolean_to_string(boolean_t *boolean);

typedef struct _prefix_t {
  token_t *operator;
  expression_t *operand;
} prefix_t;

prefix_t *prefix_new(token_t *token, expression_t *operand);
void prefix_destroy(prefix_t **p_p);
char *prefix_to_string(prefix_t *prefix);

typedef struct _infix_t {
  expression_t *left;
  token_t *operator;
  expression_t *right;
} infix_t;

infix_t *infix_new(token_t *operator, expression_t * left,
                   expression_t * right);
void infix_destroy(infix_t **i_p);
char *infix_to_string(infix_t *infix);

/* forward declaration at the top */
struct _expression_t {
  EXPRESSION_TYPE type;
  union {
    /* expressions */
    identifier_t *identifier;
    integer_t *integer;
    boolean_t *boolean;
    prefix_t *prefix;
    infix_t *infix;
  } expression;
};

expression_t *expression_new(EXPRESSION_TYPE type, void *expression);
void expression_destroy(expression_t **e_p);
char *expression_to_string(expression_t *expression);

typedef enum {
  LET_STATEMENT,
  RETURN_STATEMENT,
  EXPRESSION_STATEMENT,
} STATEMENT_TYPE;

typedef struct _let_statement_t {
  token_t *token;
  identifier_t *name;
  expression_t *value;
} let_statement_t;

let_statement_t *let_statement_new(token_t *token, identifier_t *name,
                                   expression_t *value);
void let_statement_destroy(let_statement_t **l_p);
char *let_statement_to_string(let_statement_t *let_statement);

typedef struct _return_statement_t {
  token_t *token;
  expression_t *return_value;
} return_statement_t;

return_statement_t *return_statement_new(token_t *token,
                                         expression_t *return_value);
void return_statement_destroy(return_statement_t **r_p);
char *return_statement_to_string(return_statement_t *return_statement);

typedef struct _expression_statment_t {
  token_t *token;
  expression_t *expression;
} expression_statement_t;

expression_statement_t *expression_statement_new(token_t *token,
                                                 expression_t *expression);
void expression_statement_destroy(expression_statement_t **e_p);
char *
expression_statement_to_string(expression_statement_t *expression_statement);

/* forward declaration at the top */
struct _statement_t {
  STATEMENT_TYPE type;
  union {
    let_statement_t *let_statement;
    return_statement_t *return_statement;
    expression_statement_t *expression_statement;
  } statement;
};
statement_t *statement_new(void *statement, STATEMENT_TYPE st);
void statement_destroy(statement_t **s_p, STATEMENT_TYPE st);
char *statement_to_string(statement_t *statement);

typedef struct _program_t {
  statement_t **statements;
  size_t len;
} program_t;

program_t *program_new();
void program_destroy(program_t **p_p);
void program_append_statement(program_t *program, statement_t *statement);
char *program_to_string(program_t *program);

char *get_token_literal(token_t *tok);

#endif
