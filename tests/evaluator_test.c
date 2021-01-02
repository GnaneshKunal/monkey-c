#include "../src/evaluator.h"
#include "../src/lexer.h"
#include "../src/object.h"
#include "../src/parser.h"
#include "utils.h"
#include <check.h>

typedef struct {
  parser_t *parser;
  program_t *program;
  obj_t *obj;
} test_eval_t;

test_eval_t *make_eval(parser_t *parser, program_t *program, obj_t *obj) {
  test_eval_t *eval_obj = malloc(sizeof(*eval_obj));
  eval_obj->parser = parser;
  eval_obj->program = program;
  eval_obj->obj = obj;
  return eval_obj;
}

void eval_destroy(test_eval_t **eval_obj_p) {
  assert(eval_obj_p);
  if (*eval_obj_p) {
    test_eval_t *eval_obj = *eval_obj_p;
    obj_destroy(&eval_obj->obj);
    program_destroy(&eval_obj->program);
    parser_destroy(&eval_obj->parser);
    free(eval_obj);
    *eval_obj_p = NULL;
  }
}

test_eval_t *_test_eval(const char *input) {
  lexer_t *lexer = lexer_new(input);
  parser_t *parser = parser_new(lexer);
  program_t *program = parser_parse_program(parser);

  obj_t *obj = eval(program);

  return make_eval(parser, program, obj);
}

void _test_obj_type(obj_t *obj, OBJ_TYPE ot) {

  ck_assert_msg(obj != NULL, "Object is NULL");

  ck_assert_msg(obj->type == ot, "Expected %s, got=%s", obj_type_to_str(ot),
                obj_type_to_str(obj->type));
}

void _test_int_obj(obj_t *obj, int32_t expected) {
  _test_obj_type(obj, INT_OBJ);

  ck_assert_msg(obj->int_obj->value == expected,
                "Expected=%" PRId32 ", got=%" PRId32, expected,
                obj->int_obj->value);
}

typedef struct {
  char *input;
  int32_t expected;
} test_int_obj_t;

test_int_obj_t int_test_data[] = {
    {"5", 5},
    {"10", 10},
    {"-5", -5},
    {"-10", -10},
    {"5 + 5", 10},
    {"5 + 5 + 5 + 5 - 10", 10},
    {"2 * 2 * 2 * 2 * 2", 32},
    {"-50 + 100 + -50", 0},
    {"5 * 2 + 10", 20},
    {"5 + 2 * 10", 25},
    {"20 + 2 * -10", 0},
    {"50 / 2 * 2 + 10", 60},
    {"2 * (5 + 10)", 30},
    {"3 * 3 * 3 + 10", 37},
    {"3 * (3 * 3) + 10", 37},
    {"(5 + 10 * 2 + 15 / 3) * 2 + -10", 50},
};

START_TEST(test_eval_integer_expression_loop) {
  test_eval_t *eval_obj = _test_eval(int_test_data[_i].input);

  _test_int_obj(eval_obj->obj, int_test_data[_i].expected);

  eval_destroy(&eval_obj);
}
END_TEST

typedef struct {
  char *input;
  bool expected;
} test_booj_obj_t;

void _test_bool_obj(obj_t *obj, bool expected) {
  _test_obj_type(obj, BOOL_OBJ);

  ck_assert_msg(obj->bool_obj->value == expected,
                "Expected=%s, got=%s", get_bool_literal(expected),
                get_bool_literal(obj->bool_obj->value));
}


test_booj_obj_t bool_test_data[] = {
  {"true", true},
  {"false", false},
  {"1 < 2", true},
  {"1 > 2", false},
  {"1 < 1", false},
  {"1 > 1", false},
  {"1 == 1", true},
  {"1 != 1", false},
  {"1 == 2", false},
  {"1 != 2", true},
  {"(1 < 2) == true", true},
  {"(1 < 2) == false", false},
  {"(1 > 2) == true", false},
  {"(1 > 2) == false", true},
};

START_TEST(test_eval_boolean_expression_loop)
{
  test_eval_t *eval_obj = _test_eval(bool_test_data[_i].input);

  _test_bool_obj(eval_obj->obj, bool_test_data[_i].expected);

  eval_destroy(&eval_obj);
}
END_TEST


test_booj_obj_t t_d_bang_operator[] = {
  {"!true", false},
  {"!false", true},
  {"!5", false},
  {"!!true", true},
  {"!!false", false},
  {"!!5", true},
};
START_TEST(test_bang_operator_loop)
{
  test_eval_t *eval_obj = _test_eval(t_d_bang_operator[_i].input);

  _test_bool_obj(eval_obj->obj, t_d_bang_operator[_i].expected);

  eval_destroy(&eval_obj);
}
END_TEST

typedef struct {
  OBJ_TYPE otype;
  union {
    int_obj_t int_data;
    null_obj_t null_data;
  };
} t_obj_t;

typedef struct {
  char *input;
  t_obj_t expected;
} test_obj_t;

void _test_null_obj(obj_t *actual) {
  _test_obj_type(actual, NULL_OBJ);
}

void _test_if_expression(t_obj_t expected, obj_t *actual) {
  switch (expected.otype) {
  case NULL_OBJ:
    _test_null_obj(actual);
    break;
  case INT_OBJ:
    _test_int_obj(actual, expected.int_data.value);
    break;
  }
}

test_obj_t t_d_if_else_expression[] = {
  {"if (true) { 10; }", {.otype=INT_OBJ, .int_data={.value=10}}},
  {"if (false) { 10 }", {.otype=NULL_OBJ}},
  {"if (1) { 10 }", {.otype=INT_OBJ, .int_data={.value=10}}},
  {"if (1 < 2) { 10 }", {.otype=INT_OBJ, .int_data={.value=10}}},
  {"if (1 > 2) { 10 }", {.otype=NULL_OBJ}},
  {"if (1 > 2) { 10 } else { 20 }", {.otype=INT_OBJ, .int_data={.value=20}}},
  {"if (1 < 2) { 10 } else { 20 }", {.otype=INT_OBJ, .int_data={.value=10}}},
  {"if (1 < 2) { if (10 > 1) { return 10; } } else { 20 }", {.otype=INT_OBJ, .int_data={.value=10}}},
};

START_TEST(test_if_else_expression_loop)
{
  test_eval_t *eval_obj = _test_eval(t_d_if_else_expression[_i].input);

  _test_if_expression(t_d_if_else_expression[_i].expected, eval_obj->obj);

  eval_destroy(&eval_obj);
}
END_TEST

test_obj_t t_d_return_statement[] = {
  {"return 10;", {.otype=INT_OBJ, .int_data={.value=10}}},
  {"return 10; 9;", {.otype=INT_OBJ, .int_data={10}}},
  {"return 2 * 5; 9;", {.otype=INT_OBJ, .int_data={10}}},
  {"9; return 2 * 5; 9;", {.otype=INT_OBJ, .int_data={10}}},
  {
    "if (10 > 1) { if (10 > 1) { return 10; } return 1; }",
    {.otype=INT_OBJ, .int_data={10}},
  }
};

START_TEST(test_return_statement_loop)
{
  test_eval_t *eval_obj = _test_eval(t_d_return_statement[_i].input);

  _test_obj_type(eval_obj->obj, INT_OBJ);

  _test_int_obj(eval_obj->obj, t_d_return_statement[_i].expected.int_data.value);

  eval_destroy(&eval_obj);
}
END_TEST

typedef struct {
  char *input;
  char *expected_message;
} test_error_obj_t;

test_error_obj_t t_d_error_obj[] = {
  {"5 + true;", "type mismatch: INTEGER + BOOLEAN"},
  {"5 + true; 5;", "type mismatch: INTEGER + BOOLEAN"},
  {"-true", "unknown operator: -BOOLEAN"},
  {"true + false", "unknown operator: BOOLEAN + BOOLEAN"},
  {"5; true + false; 5", "unknown operator: BOOLEAN + BOOLEAN"},
  {"if (10 > 1) { true + false; }", "unknown operator: BOOLEAN + BOOLEAN"},
  {"if (10 > 1) { if (10 > 1) { return true + false; } return 1; }", "unknown operator: BOOLEAN + BOOLEAN"},
};

_test_error_obj(char *expected_message, obj_t *error_obj) {
  _test_obj_type(error_obj, ERROR_OBJ);

  ck_assert_msg(strcmp(expected_message, error_obj->error_obj->message) == 0,
                "Expected=%s, got=%s", expected_message,
                error_obj->error_obj->message);
}

START_TEST(test_error_obj_loop)
{
  test_eval_t *eval_obj = _test_eval(t_d_error_obj[_i].input);

  _test_error_obj(t_d_error_obj[_i].expected_message, eval_obj->obj);
  eval_destroy(&eval_obj);
}
END_TEST

Suite *evaluator_suite(void) {
  Suite *s;
  TCase *tc_core;

  s = suite_create("Evaluator");
  tc_core = tcase_create("Core");

  tcase_add_loop_test(tc_core, test_eval_integer_expression_loop, 0,
                      sizeof(int_test_data) / sizeof(*int_test_data));
  tcase_add_loop_test(tc_core, test_eval_boolean_expression_loop, 0,
                      sizeof(bool_test_data) / sizeof(*bool_test_data));

  tcase_add_loop_test(tc_core, test_bang_operator_loop,
                      0, sizeof(t_d_bang_operator) / sizeof(*t_d_bang_operator));
  tcase_add_loop_test(tc_core, test_if_else_expression_loop,
                      0, sizeof(t_d_if_else_expression) / sizeof(*t_d_if_else_expression));

  tcase_add_loop_test(tc_core, test_return_statement_loop,
                      0, sizeof(t_d_return_statement) / sizeof(*t_d_return_statement));

  tcase_add_loop_test(tc_core, test_error_obj_loop,
                      0, sizeof(t_d_error_obj) / sizeof(*t_d_error_obj));

  suite_add_tcase(s, tc_core);

  return s;
}

int main(void) {
  int number_failed;
  Suite *s;
  SRunner *sr;

  s = evaluator_suite();
  sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
