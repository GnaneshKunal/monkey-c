#include "evaluator.h"
#include "ast.h"
#include "object.h"

obj_t *eval(program_t *program) {
  return eval_statements(program->len, program->statements);
}

obj_t *eval_statements(size_t len, statement_t **statements) {
  obj_t *obj = NULL;
  for (int i = 0; i < len; i++) {
    obj = eval_statement(statements[i]);
    if (obj->type == RETURN_VALUE_OBJ) {
      return obj;
    }
  }
  return obj;
}

obj_t *eval_statement(statement_t *statement) {
  switch (statement->type) {
  case EXPRESSION_STATEMENT:
    return eval_expression(statement->expression_statement->expression);
  case BLOCK_STATEMENT:
    return eval_statements(statement->block_statement->statements_len, statement->block_statement->statements);
  case RETURN_STATEMENT:
    return eval_return_statement(statement->return_statement);
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

obj_t *eval_block_statement(block_statement_t *block_statement) {
  return eval_statements(block_statement->statements_len, block_statement->statements);
}

obj_t *eval_if_expression(expression_t *expression) {
  assert(expression);
  assert(expression->type == IF_EXP);

  if_exp_t *exp = expression->if_exp;
  obj_t *condition = eval_expression(exp->condition);

  if (is_truthy(condition)) {
    obj_destroy(&condition);
    return eval_block_statement(exp->consequence);
  } else if (exp->alternative != NULL) {
    obj_destroy(&condition);
    return eval_block_statement(exp->alternative);
  } else {
    obj_destroy(&condition);
    return &NULL_IMPL_OBJ;
  }
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
  case IF_EXP:
    return eval_if_expression(expression);
  }
  return &NULL_IMPL_OBJ;
}

obj_t *eval_return_statement(return_statement_t *return_statement) {
  obj_t *value = eval_expression(return_statement->return_value);
  return obj_new(RETURN_VALUE_OBJ, return_obj_new(value));
}
