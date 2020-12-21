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

obj_t *eval_bang_operator(obj_t *right) {
  switch (right->type) {
  case BOOL_OBJ:
    return right->bool_obj->value ? &FALSE_IMPL_OBJ : &TRUE_IMPL_OBJ;
  case NULL_OBJ:
    return &TRUE_IMPL_OBJ;
  default:
    /*
     * TODO: verify the below claim.
     * Destroying the object since it won't be used.
     */
    obj_destroy(&right);
    return &FALSE_IMPL_OBJ;
  }
}

obj_t *eval_minus_operator(obj_t *right) {
  if (right->type != INT_OBJ) {
    return &NULL_IMPL_OBJ;
  }

  int32_t value = -right->int_obj->value;
  obj_destroy(&right);
  return obj_new(INT_OBJ, int_obj_new(value));
}

obj_t *eval_prefix_operation(char *operator, obj_t *right) {
  if (strcmp(operator, "!") == 0) {
    return eval_bang_operator(right);
  } else if (strcmp(operator, "-") == 0) {
    return eval_minus_operator(right);
  }

  return &NULL_IMPL_OBJ;
}

obj_t *eval_integer_infix_expression(char *operator, obj_t *left, obj_t *right) {
  int32_t left_value = left->int_obj->value;
  int32_t right_value = right->int_obj->value;

  obj_destroy(&left);
  obj_destroy(&right);

  if (strcmp(operator, "+") == 0) {
    return obj_new(INT_OBJ, int_obj_new(left_value + right_value));
  } else if (strcmp(operator, "-") == 0) {
    return obj_new(INT_OBJ, int_obj_new(left_value - right_value));
  } else if (strcmp(operator, "*") == 0) {
    return obj_new(INT_OBJ, int_obj_new(left_value * right_value));
  } else if (strcmp(operator, "/") == 0) {
    return obj_new(INT_OBJ, int_obj_new(left_value / right_value));
  } else if (strcmp (operator, ">") == 0) {
    return native_bool_to_boolean_obj(left_value > right_value);
  } else if (strcmp (operator, "<") == 0) {
    return native_bool_to_boolean_obj(left_value < right_value);
  } else if (strcmp (operator, "==") == 0) {
    return native_bool_to_boolean_obj(left_value == right_value);
  } else if (strcmp (operator, "!=") == 0) {
    return native_bool_to_boolean_obj(left_value != right_value);
  } else {
    return &NULL_IMPL_OBJ;
  }
}

obj_t *eval_infix_operation(char *operator, obj_t *left, obj_t *right) {
  if (left->type == INT_OBJ && right->type == INT_OBJ) {
    return eval_integer_infix_expression(operator, left, right);
  } else if (strcmp(operator, "==") == 0) {
    return native_bool_to_boolean_obj(left == right);
  } else if (strcmp(operator, "!=") == 0) {
    return native_bool_to_boolean_obj(left != right);
  }


  return &NULL_IMPL_OBJ;
}

obj_t *eval_expression(expression_t *expression) {
  obj_t *left = NULL;
  obj_t *right = NULL;
  switch (expression->type) {
  case INT_EXP:
    return obj_new(INT_OBJ, int_obj_new(expression->integer->value));
  case BOOLEAN_EXP:
    return native_bool_to_boolean_obj(expression->boolean->value);
  case PREFIX_EXP:
    right = eval_expression(expression->prefix->operand);
    return eval_prefix_operation(expression->prefix->operator->literal, right);
  case INFIX_EXP:
    left = eval_expression(expression->infix->left);
    right = eval_expression(expression->infix->right);
    return eval_infix_operation(expression->infix->operator->literal, left, right);
  }
  return &NULL_IMPL_OBJ;
}
