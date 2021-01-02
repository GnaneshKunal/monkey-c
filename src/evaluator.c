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
      obj_t *value = obj->return_obj->value;
      /*
       * Destroy return object but not its value
       */
      free(obj->return_obj);
      free(obj);
      obj = NULL;
      return value;
    } else if (obj->type == ERROR_OBJ) {
      return obj;
    }
    if (i != (len - 1))
      obj_destroy(&obj);
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
    char *str = NULL;
    obj_t *error_obj = NULL;
    asprintf(&str, "unknown operator: -%s", obj_type_to_str(right->type));
    obj_destroy(&right);
    error_obj = make_error(str);
    free(str);
    return error_obj;
  }
  int32_t value = -right->int_obj->value;
  obj_destroy(&right);
  return obj_new(INT_OBJ, int_obj_new(value));
}

obj_t *eval_prefix_operation(const char *operator, obj_t *right) {
  if (strcmp(operator, "!") == 0) {
    return eval_bang_operator(right);
  } else if (strcmp(operator, "-") == 0) {
    return eval_minus_operator(right);
  }
  return &NULL_IMPL_OBJ;
}

obj_t *eval_integer_infix_expression(const char *operator, obj_t *left, obj_t *right) {
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
    char *str = NULL;
    obj_t *error_obj = NULL;
    asprintf(&str, "unknown operator: %s %s %s", obj_type_to_str(left->type), operator, obj_type_to_str(right->type));
    obj_destroy(&left);
    obj_destroy(&right);
    error_obj = make_error(str);
    free(str);
    return error_obj;
  }
}

obj_t *eval_infix_operation(const char *operator, obj_t *left, obj_t *right) {
  char *str = NULL;
  obj_t *error_obj = NULL;
  if (left->type == INT_OBJ && right->type == INT_OBJ) {
    return eval_integer_infix_expression(operator, left, right);
  } else if (strcmp(operator, "==") == 0) {
    return native_bool_to_boolean_obj(left == right);
  } else if (strcmp(operator, "!=") == 0) {
    return native_bool_to_boolean_obj(left != right);
  } else if (left->type != right->type) {
    asprintf(&str, "type mismatch: %s %s %s", obj_type_to_str(left->type), operator, obj_type_to_str(right->type));
    error_obj = make_error(str);
  } else {
    asprintf(&str, "unknown operator: %s %s %s", obj_type_to_str(left->type), operator, obj_type_to_str(right->type));
    error_obj = make_error(str);
  }
  obj_destroy(&left);
  obj_destroy(&right);
  assert(str);
  free(str);
  return error_obj;
}

obj_t *eval_block_statement(block_statement_t *block_statement) {
  size_t len = block_statement->statements_len;
  statement_t **statements = block_statement->statements;
  obj_t *obj = NULL;
  for (int i = 0; i < len; i++) {
    obj = eval_statement(statements[i]);
    if (obj->type == RETURN_VALUE_OBJ) {
      return obj;
    } else if (obj->type == ERROR_OBJ) {
      return obj;
    }
    if (i != (len - 1)) {
      obj_destroy(&obj);
    }
  }
  return obj;
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

obj_t *make_error(const char *str) {
  assert(str);
  return obj_new(ERROR_OBJ, error_obj_new(str));
}
