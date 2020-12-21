#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "utils.h"
#include "ast.h"
#include "object.h"

obj_t *eval(program_t *program);
obj_t *eval_statement(statement_t *statement);
obj_t *eval_expression(expression_t *expression);

obj_t *eval_bang_operator(obj_t *right);
obj_t *eval_prefix_operation(char *operator, obj_t *right);
obj_t *eval_minus_operator(obj_t *right);
#endif
