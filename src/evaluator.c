#include "evaluator.h"
#include "ast.h"
#include "object.h"

obj_t *eval(program_t *program) {
  obj_t *obj = NULL;
  for (int i = 0; i < program->len; i++) {
    obj = eval_statement(program->statements[i]);
  }
  return obj;
}

obj_t *eval_statement(statement_t *statement) {
  switch (statement->type) {
  case EXPRESSION_STATEMENT:
    return eval_expression(statement->expression_statement->expression);
  }
  return NULL;
}

obj_t *eval_expression(expression_t *expression) {
  switch (expression->type) {
  case INT_EXP:
    return obj_new(INT_OBJ, int_obj_new(expression->integer->value));
  }
  return NULL;
}
