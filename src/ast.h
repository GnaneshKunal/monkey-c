#ifndef AST_H
#define AST_H

#include "token.h"
#include "utils.h"

typedef struct _expression_t expression_t;
typedef struct _statement_t statement_t;

typedef struct _block_statement_t block_statement_t;

typedef enum {
  IDENT_EXP,
  INT_EXP,
  BOOLEAN_EXP,
  PREFIX_EXP,
  INFIX_EXP,
  IF_EXP,
  FN_EXP,
  CALL_EXP,
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

typedef struct _if_exp_t {
  token_t *token; /* The 'if' token */
  expression_t *condition;
  block_statement_t *consequence;
  block_statement_t *alternative;
} if_exp_t;

if_exp_t *if_exp_new(token_t *token, expression_t *condition,
                     block_statement_t *consequence,
                     block_statement_t *alternative);
void if_exp_destroy(if_exp_t **i_p);
char *if_exp_to_string(if_exp_t *if_exp);

typedef struct _param_t {
  identifier_t **parameters;
  size_t len;
} param_t;

param_t *param_new(void);
void param_append(param_t *params, identifier_t *identifier);
void param_destroy(param_t **params);
char *param_to_string(param_t *params);

typedef struct _fn_t {
  token_t *token; /* 'fn' token */
  param_t *params;
  block_statement_t *body;
} fn_t;

fn_t *fn_new(token_t *token, param_t *params, block_statement_t *body);
void fn_destroy(fn_t **function_literal);
char *fn_to_string(fn_t *functional_literal);

typedef struct _param_exp_t {
  expression_t **expressions;
  size_t len;
} param_exp_t;

param_exp_t *param_exp_new(void);
void param_exp_append(param_exp_t *param_exps, expression_t *exp);
void param_exp_destroy(param_exp_t **param_exps);
char *param_exp_to_string(param_exp_t *param_exps);

typedef struct _call_exp_t {
  token_t *token;               /* The '(' token */
  expression_t *call_exp;       /* Identifier or function literal */
  param_exp_t *param_exps;
} call_exp_t;

call_exp_t *call_exp_new(token_t *token, param_exp_t *param_exps, expression_t *call_exp);
void call_exp_destroy(call_exp_t **c_p);
char *call_exp_to_string(call_exp_t *call_exp);

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
    if_exp_t *if_exp;
    fn_t *fn;
    call_exp_t *call_exp;
  };
};

expression_t *expression_new(EXPRESSION_TYPE type, void *expression);
void expression_destroy(expression_t **e_p);
char *expression_to_string(expression_t *expression);

typedef enum {
  LET_STATEMENT,
  RETURN_STATEMENT,
  EXPRESSION_STATEMENT,
  BLOCK_STATEMENT,
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

struct _block_statement_t {
  token_t *token; /* the '{' token */
  statement_t **statements;
  size_t statements_len;
};

block_statement_t *block_statement_new(token_t *token, statement_t **statements,
                                       size_t statements_len);
void block_statement_destroy(block_statement_t **b_p);
char *block_statement_to_string(block_statement_t *block_statement);

/* forward declaration at the top */
struct _statement_t {
  STATEMENT_TYPE type;
  union {
    let_statement_t *let_statement;
    return_statement_t *return_statement;
    expression_statement_t *expression_statement;
    block_statement_t *block_statement;
  };
};
statement_t *statement_new(void *statement, STATEMENT_TYPE st);
void statement_destroy(statement_t **s_p);
char *statement_to_string(statement_t *statement);

typedef struct _program_t {
  statement_t **statements;
  size_t len;
} program_t;

program_t *program_new(void);
void program_destroy(program_t **p_p);
void program_append_statement(program_t *program, statement_t *statement);
char *program_to_string(program_t *program);

const char *expression_type_to_str(EXPRESSION_TYPE et);
const char *statement_type_to_str(STATEMENT_TYPE st);
#endif
