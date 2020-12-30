#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "utils.h"
#include "ast.h"
#include "object.h"

obj_t *eval(program_t *program);
obj_t *eval_statements(size_t len, statement_t **statements);
obj_t *eval_statement(statement_t *statement);
obj_t *eval_expression(expression_t *expression);
obj_t *eval_if_expression(expression_t *expression);

obj_t *eval_bang_operator(obj_t *right);
obj_t *eval_minus_operator(obj_t *right);
obj_t *eval_prefix_operation(char *operator, obj_t *right);
obj_t *eval_infix_operation(char *operator, obj_t *left, obj_t *right);
obj_t *eval_integer_infix_expression(char *operator, obj_t *left, obj_t *right);

obj_t *eval_block_statement(block_statement_t *block_statement);
obj_t *eval_return_statement(return_statement_t *return_statement);
#endif
