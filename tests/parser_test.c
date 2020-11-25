#include "../src/parser.h"
#include <check.h>

void _test_statement_type(statement_t *statement, STATEMENT_TYPE st) {
  ck_assert_msg(statement->type == st, "Expected statement=%s, Got=%s\n",
                statement_type_to_str(st),
                statement_type_to_str(statement->type));
}

void _test_expression_type(expression_t *expression, EXPRESSION_TYPE et) {
  ck_assert_msg(expression->type == et, "Expected expression=%s, Got=%s\n",
                expression_type_to_str(et),
                expression_type_to_str(expression->type));
}

void _test_str_literal(char *actual_literal, const char *expected_literal) {
  ck_assert_msg(strcmp(expected_literal, actual_literal) == 0,
                "Expected=%s, Got=%s\n", expected_literal, actual_literal);
}

void _test_let_statement(statement_t *s, char *name) {

  _test_statement_type(s, LET_STATEMENT);

  let_statement_t *let = (let_statement_t *)s->let_statement;
  ck_assert_msg(let != NULL, "let statement is NULL");

  _test_str_literal(let->token->literal, "let");

  ck_assert_msg(strcmp(let->name->value, name) == 0,
                "let statement value not '%s'. Got=%s\n", name,
                let->name->value);

  _test_str_literal(let->name->token->literal, name);
}

void _test_integer_literal(expression_t *expression, int32_t value) {
  _test_expression_type(expression, INT_EXP);

  integer_t *integer = expression->integer;
  ck_assert_msg(integer->value == value,
                "int.Value not %" PRId32 ", Got=%" PRId32, value,
                integer->value);

  char int_str[20];
  sprintf(int_str, "%" PRId32, value);
  _test_str_literal(integer->token->literal, int_str);
}

void _test_boolean_literal(expression_t *expression, bool value) {
  _test_expression_type(expression, BOOLEAN_EXP);

  boolean_t *boolean = expression->boolean;
  ck_assert_msg(boolean->value == value, "Expected=%s, Got=%s",
                get_bool_literal(value), get_bool_literal(boolean->value));

  _test_str_literal(boolean->token->literal, get_bool_literal(value));
}

void _test_literal(EXPRESSION_TYPE et, expression_t *expression,
                   uintptr_t *value) {
  switch (et) {
  case INT_EXP:
    _test_integer_literal(expression, (int32_t)value);
    return;
  case BOOLEAN_EXP:
    _test_boolean_literal(expression, (bool)value);
    return;
  default:
    ck_abort_msg("Unknown literal type (%d)", et);
  }
}

bool check_parser_errors(parser_t *parser) {
  size_t error_len = 0;
  char **errors = parser_get_errors(parser, &error_len);
  if (error_len > 0) {

    printf("parser has %zu errors\n", error_len);

    int i = 0;
    while (i < error_len) {
      puts(errors[i]);
      free(errors[i]);
      i++;
    }
    free(errors);
    return true;
  }
  return false;
};

START_TEST(test_let_statements) {
  const char input[] = "                              \
let x = 5;                                            \
let y = 10;                                           \
let foobar = 838383;                                  \
";

  lexer_t *l = lexer_new(input);
  parser_t *p = parser_new(l);

  program_t *program = parser_parse_program(p);
  if (check_parser_errors(p)) {
    program_destroy(&program);
    parser_destroy(&p); /* destroys lexer too */
    ck_abort_msg("Program has got errors");
    return;
  }

  ck_assert_msg(program != NULL, "parse_program returned NULL");

  ck_assert_msg(program->len == 3,
                "program.Statements does not contain 3 statements. Got=%lu\n",
                program->len);

  typedef struct {
    char *expected_identifier;
  } test_identifier_t;

  test_identifier_t tests[] = {
      {"x"},
      {"y"},
      {"foobar"},
  };

  size_t tests_size = sizeof(tests) / sizeof(*tests);

  for (size_t i = 0; i < tests_size; i++) {
    statement_t *statement = program->statements[i];
    _test_let_statement(statement, tests[i].expected_identifier);
  }

  program_destroy(&program);
  parser_destroy(&p); /* destroys lexer too */
}
END_TEST

START_TEST(test_return_statements) {
  const char input[] = "                              \
return 5;                                             \
 return 10;                                           \
 return 993322;                                       \
";

  lexer_t *lexer = lexer_new(input);
  parser_t *parser = parser_new(lexer);
  program_t *program = parser_parse_program(parser);
  if (check_parser_errors(parser)) {
    program_destroy(&program);
    parser_destroy(&parser);
    return;
  }

  if (program == NULL) {
    parser_destroy(&parser);
    ck_abort_msg("parse_program returned NULL");
  }

  if (program->len != 3) {
    program_destroy(&program);
    parser_destroy(&parser);
    ck_abort_msg("Expected statements %d. Got=%d\n", 3, program->len);
  }

  for (int i = 0; i < program->len; i++) {
    statement_t *statement = program->statements[i];

    _test_statement_type(statement, RETURN_STATEMENT);

    return_statement_t *return_statement = statement->return_statement;

    _test_str_literal(return_statement->token->literal, "return");
  }

  program_destroy(&program);
  parser_destroy(&parser); /* destroys lexer too */
}
END_TEST

START_TEST(test_identifier_expressions) {
  const char *input = "foobar;";

  lexer_t *lexer = lexer_new(input);
  parser_t *parser = parser_new(lexer);
  program_t *program = parser_parse_program(parser);

  if (check_parser_errors(parser)) {
    program_destroy(&program);
    parser_destroy(&parser);
    ck_abort_msg("Program has got errors");
  }

  ck_assert_msg(program != NULL, "parse_program returned NULL");

  ck_assert_msg(program->len == 1,
                "program has not enough statements. Expected=%lu, Got=%lu", 1,
                program->len);

  statement_t *statement = program->statements[0];
  _test_statement_type(statement, EXPRESSION_STATEMENT);

  expression_statement_t *expression_statement =
      statement->expression_statement;
  expression_t *expression = expression_statement->expression;

  _test_expression_type(expression, IDENT_EXP);

  identifier_t *identifier = expression->identifier;
  _test_str_literal(identifier->value, "foobar");

  _test_str_literal(identifier->token->literal, "foobar");

  program_destroy(&program);
  parser_destroy(&parser);
}
END_TEST

START_TEST(test_integer_literal_expression) {

  const char *input = "5;";

  lexer_t *lexer = lexer_new(input);
  parser_t *parser = parser_new(lexer);
  program_t *program = parser_parse_program(parser);

  if (check_parser_errors(parser)) {
    program_destroy(&program);
    parser_destroy(&parser);
    ck_abort_msg("Program has got errors");
    return;
  }

  ck_assert_msg(program != NULL, "parse_program returned NULL");

  ck_assert_msg(program->len == 1,
                "Program has not enought statements. Expected=%ld, Got=%ld\n",
                1, program->len);

  statement_t *statement = program->statements[0];
  _test_statement_type(statement, EXPRESSION_STATEMENT);

  expression_statement_t *expression_statement =
      statement->expression_statement;
  _test_integer_literal(expression_statement->expression, 5);

  program_destroy(&program);
  parser_destroy(&parser);
}
END_TEST

typedef struct _prefix_result_t {
  char *input;
  char *operator;
  EXPRESSION_TYPE et;
  union {
    int32_t integer_value;
    bool boolean_value;
  };
} prefix_result_t;

prefix_result_t prefix_tests[] = {
    {"!5;", "!", INT_EXP, {5}},
    {"-15;", "-", INT_EXP, {15}},
    {"!true;", "!", BOOLEAN_EXP, {.boolean_value = true}},
    {"!false;", "!", BOOLEAN_EXP, {.boolean_value = false}}};

START_TEST(test_parsing_prefix_expression_loop) {

  prefix_result_t test = prefix_tests[_i];

  lexer_t *lexer = lexer_new(test.input);
  parser_t *parser = parser_new(lexer);
  program_t *program = parser_parse_program(parser);

  ck_assert_msg(program != NULL, "parse_program returned NULL");

  ck_assert_msg(program->len == 1,
                "program.Statements does not contain enought statements. "
                "Expected=%d statements, Got=%d statements\n",
                1, program->len);

  statement_t *statement = program->statements[0];
  _test_statement_type(statement, EXPRESSION_STATEMENT);

  expression_statement_t *expression_statement =
      statement->expression_statement;
  expression_t *expression = expression_statement->expression;
  _test_expression_type(expression, PREFIX_EXP);

  prefix_t *prefix = expression->prefix;
  _test_str_literal(prefix->operator->literal, test.operator);

  _test_literal(test.et, prefix->operand,
                (uintptr_t *)(test.et == INT_EXP ? test.integer_value
                                                 : test.boolean_value));

  program_destroy(&program);
  parser_destroy(&parser);
}
END_TEST

typedef struct _infix_results_t {
  char *input;
  int32_t left_value;
  char *operator;
  int32_t right_value;
} infix_results_t;

infix_results_t infix_tests[] = {
    {"5 + 5;", 5, "+", 5},   {"5 - 5;", 5, "-", 5},   {"5 * 5;", 5, "*", 5},
    {"5 / 5;", 5, "/", 5},   {"5 > 5;", 5, ">", 5},   {"5 < 5;", 5, "<", 5},
    {"5 == 5;", 5, "==", 5}, {"5 != 5;", 5, "!=", 5},
};

START_TEST(test_parsing_infix_expression_loop) {
  lexer_t *lexer = lexer_new(infix_tests[_i].input);
  parser_t *parser = parser_new(lexer);
  program_t *program = parser_parse_program(parser);

  ck_assert_msg(program->len == 1,
                "program.statements does not contain %d statements. Got=%ld\n",
                1, program->len);

  statement_t *statement = program->statements[0];
  _test_statement_type(statement, EXPRESSION_STATEMENT);

  expression_statement_t *expression_statement =
      statement->expression_statement;
  expression_t *expression = expression_statement->expression;
  _test_expression_type(expression, INFIX_EXP);

  infix_t *infix = expression->infix;
  _test_str_literal(infix->operator->literal, infix_tests[_i].operator);

  _test_integer_literal(infix->left, infix_tests[_i].left_value);

  _test_integer_literal(infix->right, infix_tests[_i].right_value);

  program_destroy(&program);
  parser_destroy(&parser);
}
END_TEST

typedef struct _operator_precedence_tests {
  char *input;
  char *expected;
} operator_precedence_tests;

operator_precedence_tests operator_tests[] = {
    {"-a * b", "((-a) * b)"},
    {"!-a", "(!(-a))"},
    {"a + b + c", "((a + b) + c)"},
    {"a + b - c", "((a + b) - c)"},
    {"a * b * c", "((a * b) * c)"},
    {"a * b / c", "((a * b) / c)"},
    {"a + b / c", "(a + (b / c))"},
    {"a + b * c + d / e - f", "(((a + (b * c)) + (d / e)) - f)"},
    {"3 + 4; -5 * 5", "(3 + 4)((-5) * 5)"},
    {"5 > 4 == 3 < 4", "((5 > 4) == (3 < 4))"},
    {"5 < 4 != 3 > 4", "((5 < 4) != (3 > 4))"},
    {"3 + 4 * 5 == 3 * 1 + 4 * 5", "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))"},
    {"true", "true"},
    {"false", "false"},
    {"3 > 5 == false", "((3 > 5) == false)"},
    {"3 < 5 == true", "((3 < 5) == true)"},
    {"1 + (2 + 3) + 4", "((1 + (2 + 3)) + 4)"},
    {"(5 + 5) * 2", "((5 + 5) * 2)"},
    {"2 / (5 + 5)", "(2 / (5 + 5))"},
    {"-(5 + 5)", "(-(5 + 5))"},
    {"!(true == true)", "(!(true == true))"},
};

START_TEST(test_parsing_operator_precedence_loop) {
  lexer_t *lexer = lexer_new(operator_tests[_i].input);
  parser_t *parser = parser_new(lexer);
  program_t *program = parser_parse_program(parser);

  char *program_str = program_to_string(program);
  _test_str_literal(program_str, operator_tests[_i].expected);
  free(program_str);

  program_destroy(&program);
  parser_destroy(&parser);
}
END_TEST

typedef struct _boolean_infix_results_t {
  char *input;
  bool left_value;
  char *operator;
  bool right_value;
} boolean_infix_results_t;

boolean_infix_results_t boolean_infix_tests[] = {
    {"true == true", true, "==", true},
    {"true != false", true, "!=", false},
    {"false == false", false, "==", false},
};

START_TEST(test_parsing_boolean_infix_expression_loop) {
  lexer_t *lexer = lexer_new(boolean_infix_tests[_i].input);
  parser_t *parser = parser_new(lexer);
  program_t *program = parser_parse_program(parser);

  ck_assert_msg(program->len == 1,
                "program.statements does not contain %d statements. Got=%ld\n",
                1, program->len);

  statement_t *statement = program->statements[0];
  _test_statement_type(statement, EXPRESSION_STATEMENT);

  expression_statement_t *expression_statement =
      statement->expression_statement;
  expression_t *expression = expression_statement->expression;
  _test_expression_type(expression, INFIX_EXP);

  infix_t *infix = expression->infix;
  _test_str_literal(infix->operator->literal, boolean_infix_tests[_i].operator);

  _test_boolean_literal(infix->left, boolean_infix_tests[_i].left_value);

  _test_boolean_literal(infix->right, boolean_infix_tests[_i].right_value);

  program_destroy(&program);
  parser_destroy(&parser);
}
END_TEST

Suite *parser_suite(void) {
  Suite *s;
  TCase *tc_core;

  s = suite_create("Parser");
  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_let_statements);
  tcase_add_test(tc_core, test_return_statements);
  tcase_add_test(tc_core, test_identifier_expressions);
  tcase_add_test(tc_core, test_integer_literal_expression);

  size_t prefix_tests_len = sizeof(prefix_tests) / sizeof(*prefix_tests);
  tcase_add_loop_test(tc_core, test_parsing_prefix_expression_loop, 0,
                      prefix_tests_len);

  size_t infix_tests_len = sizeof(infix_tests) / sizeof(*infix_tests);
  tcase_add_loop_test(tc_core, test_parsing_infix_expression_loop, 0,
                      infix_tests_len);

  size_t operator_tests_len = sizeof(operator_tests) / sizeof(*operator_tests);
  tcase_add_loop_test(tc_core, test_parsing_operator_precedence_loop, 0,
                      operator_tests_len);

  size_t boolean_infix_tests_len =
      sizeof(boolean_infix_tests) / sizeof(*boolean_infix_tests);
  tcase_add_loop_test(tc_core, test_parsing_boolean_infix_expression_loop, 0,
                      boolean_infix_tests_len);

  suite_add_tcase(s, tc_core);

  return s;
}

int main(void) {

  int number_failed;
  Suite *s;
  SRunner *sr;

  s = parser_suite();
  sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
