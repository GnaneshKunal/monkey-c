#include "../src/evaluator.h"
#include "../src/parser.h"
#include "../src/lexer.h"
#include "../src/object.h"
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
  ck_assert_msg(obj->type == ot,
                "Expected %s, got=%s\n", obj_type_to_str(ot), obj_type_to_str(obj->type));
}

void _test_int_obj(obj_t *obj, int32_t expected) {
  _test_obj_type(obj, INT_OBJ);

  ck_assert_msg(obj->int_obj->value == expected,
                "Expected=%" PRId32 ", got=%" PRId32 "\n",
                expected, obj->int_obj->value);
}

typedef struct {
  char *input;
  int32_t expected;
} test_int_obj_t;

test_int_obj_t int_test_data[] = {
  {"5", 5},
  {"10", 10},
};

START_TEST(test_eval_integer_expression_loop)
{
  test_eval_t *eval_obj = _test_eval(int_test_data[_i].input);

  _test_int_obj(eval_obj->obj, int_test_data[_i].expected);

  eval_destroy(&eval_obj);
}
END_TEST

Suite *evaluator_suite(void) {
  Suite *s;
  TCase *tc_core;

  s = suite_create("Evaluator");
  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_eval_integer_expression_loop);

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
