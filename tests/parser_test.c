#include "../src/parser.h"
#include <check.h>

typedef enum {
  IDENT_DT,
  INT_DT,
  BOOL_DT,
} TEST_DATA_TYPE;

typedef struct {
  TEST_DATA_TYPE dt;
  union {
    char *ident_data;
    int32_t int_data;
    bool bool_data;
  };
} test_data_t;

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

void _test_ident_literal(expression_t *expression, char *expected_identifier) {
  _test_str_literal(expression->identifier->value, expected_identifier);
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

void _test_infix(infix_t *infix, char *expected_operator, test_data_t left,
                 test_data_t right) {
  _test_str_literal(infix->operator->literal, expected_operator);

  switch (left.dt) {
  case IDENT_DT:
    _test_ident_literal(infix->left, left.ident_data);
    break;
  case INT_DT:
    _test_integer_literal(infix->left, left.int_data);
    break;
  case BOOL_DT:
    _test_boolean_literal(infix->left, left.bool_data);
    break;
  default:
    ck_abort_msg("Unknown infix type (%d)", left.dt);
  }

  switch (right.dt) {
  case IDENT_DT:
    _test_ident_literal(infix->right, right.ident_data);
    break;
  case INT_DT:
    _test_integer_literal(infix->right, right.int_data);
    break;
  case BOOL_DT:
    _test_boolean_literal(infix->right, right.bool_data);
    break;
  default:
    ck_abort_msg("Unknown infix type (%d)", right.dt);
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
  _test_infix(infix,
              infix_tests[_i].operator,(
                  test_data_t){INT_DT, .int_data = infix_tests[_i].left_value},
              (test_data_t){INT_DT, .int_data = infix_tests[_i].right_value});

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
    {
      "a + add(b * c) + d",
      "((a + add((b * c))) + d)"
    },
    {
      "add(a, b, 1, 2 * 3, 4 + 5, add(6, 7 * 8))",
      "add(a, b, 1, (2 * 3), (4 + 5), add(6, (7 * 8)))"
    },
    {
      "add(a + b + c * d / f + g)",
      "add((((a + b) + ((c * d) / f)) + g))"
    }
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
  _test_infix(
      infix, boolean_infix_tests[_i].operator,(
                 test_data_t){BOOL_DT,
                              .bool_data = boolean_infix_tests[_i].left_value},
      (test_data_t){BOOL_DT, .bool_data = boolean_infix_tests[_i].right_value});

  program_destroy(&program);
  parser_destroy(&parser);
}
END_TEST

START_TEST(test_if_expression) {
  const char *input = "if (x < y) { x }";

  lexer_t *lexer = lexer_new(input);
  parser_t *parser = parser_new(lexer);
  program_t *program = parser_parse_program(parser);
  if (check_parser_errors(parser)) {
    program_destroy(&program);
    parser_destroy(&parser); /* destroys lexer too */
    ck_abort_msg("Program has got errors");
    return;
  }

  ck_assert_msg(program->len == 1,
                "program.statements does not contain %d statements. Got=%ld\n",
                1, program->len);

  statement_t *statement = program->statements[0];
  _test_statement_type(statement, EXPRESSION_STATEMENT);

  expression_statement_t *expression_statement =
      statement->expression_statement;
  expression_t *expression = expression_statement->expression;
  _test_expression_type(expression, IF_EXP);

  if_exp_t *if_exp = expression->if_exp;
  expression_t *condition = if_exp->condition;
  _test_expression_type(condition, INFIX_EXP);

  infix_t *condition_core = condition->infix;
  _test_infix(condition_core, "<", (test_data_t){IDENT_DT, .ident_data = "x"},
              (test_data_t){IDENT_DT, .ident_data = "y"});

  block_statement_t *consequence = if_exp->consequence;
  ck_assert_msg(consequence->statements_len == 1,
                "consequence is not 1 statements, Got=%d\n",
                consequence->statements_len);

  _test_statement_type(consequence->statements[0], EXPRESSION_STATEMENT);
  expression_statement_t *consequence_expression =
      consequence->statements[0]->expression_statement;
  _test_str_literal(consequence_expression->expression->identifier->value, "x");

  ck_assert_msg(if_exp->alternative == NULL, "Else part is not NULL");

  program_destroy(&program);
  parser_destroy(&parser);
}
END_TEST

START_TEST(test_if_else_expression) {
  const char *input = "if (x < y) { x } else { y }";

  lexer_t *lexer = lexer_new(input);
  parser_t *parser = parser_new(lexer);
  program_t *program = parser_parse_program(parser);
  if (check_parser_errors(parser)) {
    program_destroy(&program);
    parser_destroy(&parser); /* destroys lexer too */
    ck_abort_msg("Program has got errors");
    return;
  }

  ck_assert_msg(program->len == 1,
                "program.statements does not contain %d statements. Got=%ld\n",
                1, program->len);

  statement_t *statement = program->statements[0];
  _test_statement_type(statement, EXPRESSION_STATEMENT);

  expression_statement_t *expression_statement =
      statement->expression_statement;
  expression_t *expression = expression_statement->expression;
  _test_expression_type(expression, IF_EXP);

  if_exp_t *if_exp = expression->if_exp;
  expression_t *condition = if_exp->condition;
  _test_expression_type(condition, INFIX_EXP);

  infix_t *condition_core = condition->infix;
  _test_infix(condition_core, "<", (test_data_t){IDENT_DT, .ident_data = "x"},
              (test_data_t){IDENT_DT, .ident_data = "y"});

  block_statement_t *consequence = if_exp->consequence;
  ck_assert_msg(consequence->statements_len == 1,
                "consequence is not 1 statements, Got=%d\n",
                consequence->statements_len);

  _test_statement_type(consequence->statements[0], EXPRESSION_STATEMENT);
  expression_statement_t *consequence_expression =
      consequence->statements[0]->expression_statement;
  _test_str_literal(consequence_expression->expression->identifier->value, "x");

  block_statement_t *alternative = if_exp->alternative;
  ck_assert_msg(if_exp->alternative != NULL, "Else part is NULL");
  expression_statement_t *alternative_expression =
      alternative->statements[0]->expression_statement;
  _test_str_literal(alternative_expression->expression->identifier->value, "y");

  program_destroy(&program);
  parser_destroy(&parser);
}
END_TEST

START_TEST(test_function_literal_parsing) {
  const char *input = "fn(x, y) { x + y; }";

  lexer_t *lexer = lexer_new(input);
  parser_t *parser = parser_new(lexer);

  program_t *program = parser_parse_program(parser);
  if (check_parser_errors(parser)) {
    program_destroy(&program);
    parser_destroy(&parser); /* destroys lexer too */
    ck_abort_msg("Program has got errors");
    return;
  }

  ck_assert_msg(program->len == 1,
                "program.statements does not contain %d statements. Got=%ld\n",
                1, program->len);

  statement_t *statement = program->statements[0];
  _test_statement_type(statement, EXPRESSION_STATEMENT);

  expression_statement_t *expression_statement =
      statement->expression_statement;
  expression_t *expression = expression_statement->expression;
  _test_expression_type(expression, FN_EXP);

  fn_t *function = expression->fn;
  param_t *params = function->params;
  ck_assert_msg(params->len == 2,
                "function literal parameters wrong. want 2, got=%ld\n",
                params->len);

  _test_str_literal(params->parameters[0]->value, "x");
  _test_str_literal(params->parameters[1]->value, "y");

  block_statement_t *block_statement = function->body;
  ck_assert_msg(block_statement->statements_len == 1,
                "function body statements has not 1 statements. got=%ld\n",
                block_statement->statements_len);

  _test_statement_type(block_statement->statements[0], EXPRESSION_STATEMENT);
  expression_statement_t *b_expression_statement =
      block_statement->statements[0]->expression_statement;

  expression_t *b_expression = b_expression_statement->expression;
  _test_expression_type(b_expression, INFIX_EXP);

  infix_t *infix = b_expression->infix;
  _test_infix(infix, "+", (test_data_t){IDENT_DT, .ident_data = "x"},
              (test_data_t){IDENT_DT, .ident_data = "y"});

  program_destroy(&program);
  parser_destroy(&parser);
}
END_TEST

typedef struct {
  char *input;
  char *expected_params;
  int params_len;
} test_fn_params_t;

test_fn_params_t fn_params_tests[] = {{
                                          "fn() {};",
                                          "",
                                      },
                                      {
                                          "fn(x) {};",
                                          "x",
                                          1,
                                      },
                                      {
                                          "fn(x, y, z) {}",
                                          "x,y,z",
                                          3,
                                      }};

START_TEST(test_function_parameter_parsing_loop) {
  lexer_t *lexer = lexer_new(fn_params_tests[_i].input);
  parser_t *parser = parser_new(lexer);

  program_t *program = parser_parse_program(parser);
  if (check_parser_errors(parser)) {
    program_destroy(&program);
    parser_destroy(&parser); /* destroys lexer too */
    ck_abort_msg("Program has got errors");
    return;
  }

  ck_assert_msg(program->len == 1,
                "program.statements does not contain %d statements. Got=%ld\n",
                1, program->len);

  statement_t *statement = program->statements[0];
  _test_statement_type(statement, EXPRESSION_STATEMENT);

  expression_statement_t *expression_statement =
      statement->expression_statement;
  expression_t *expression = expression_statement->expression;
  _test_expression_type(expression, FN_EXP);

  fn_t *function = expression->fn;
  param_t *params = function->params;
  ck_assert_msg(params->len == fn_params_tests[_i].params_len,
                "function literal parameters wrong. want 2, got=%ld\n",
                params->len);

  char *rest = NULL;
  char *param;
  size_t p_idx = 0;

  char *params_str = strdup(fn_params_tests[_i].expected_params);

  for (param = strtok_r(params_str, ",", &rest); param != NULL;
       param = strtok_r(NULL, ",", &rest)) {
    _test_str_literal(params->parameters[p_idx++]->value, param);
  }
  free(params_str);

  program_destroy(&program);
  parser_destroy(&parser);
}
END_TEST

START_TEST(test_call_expression_parsing) {
  const char *input = "add(1, 2 * 3, 4 + 5);";

  lexer_t *lexer = lexer_new(input);
  parser_t *parser = parser_new(lexer);
  program_t *program = parser_parse_program(parser);
  if (check_parser_errors(parser)) {
    program_destroy(&program);
    parser_destroy(&parser); /* destroys lexer too */
    ck_abort_msg("Program has got errors");
    return;
  }

  ck_assert_msg(program->len == 1,
                "program.statements does not contain %d statements. Got=%ld\n",
                1, program->len);

  statement_t *statement = program->statements[0];
  _test_statement_type(statement, EXPRESSION_STATEMENT);

  expression_statement_t *expression_statement =
      statement->expression_statement;
  expression_t *expression = expression_statement->expression;
  _test_expression_type(expression, CALL_EXP);

  call_exp_t *call_exp = expression->call_exp;

  expression_t *expression_inner = call_exp->call_exp;

  _test_expression_type(expression_inner, IDENT_EXP);

  identifier_t *identifier = expression_inner->identifier;
  _test_str_literal(identifier->value, "add");

  param_exp_t *param_exps = call_exp->param_exps;
  ck_assert_msg(param_exps->len == 3, "wrong length of arguments. Got=%ld\n",
                param_exps->len);

  expression_t *param_1 = param_exps->expressions[0];
  _test_integer_literal(param_1, 1);

  expression_t *param_2 = param_exps->expressions[1];
  infix_t *infix_1 = param_2->infix;
  _test_infix(infix_1, "*", (test_data_t){INT_DT, .int_data = 2},
              (test_data_t){INT_DT, .int_data = 3});

  expression_t *param_3 = param_exps->expressions[2];
  infix_t *infix_2 = param_3->infix;
  _test_infix(infix_2, "+", (test_data_t){INT_DT, .int_data = 4},
              (test_data_t){INT_DT, .int_data = 5});

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

  tcase_add_test(tc_core, test_if_expression);
  tcase_add_test(tc_core, test_if_else_expression);
  tcase_add_test(tc_core, test_function_literal_parsing);

  size_t fn_params_tests_len =
      sizeof(fn_params_tests) / sizeof(*fn_params_tests);
  tcase_add_loop_test(tc_core, test_function_parameter_parsing_loop, 0,
                      fn_params_tests_len);

  tcase_add_test(tc_core, test_call_expression_parsing);

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
