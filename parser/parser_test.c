#include "parser.h"

bool test_let_statement(statement_t *s, char *name) {
  if (s->type != LET_STATEMENT) {
    return false;
  }

  let_statement_t *let = (let_statement_t *)s->statement.let_statement;
  assert(let && "let statement is NULL");
  char *literal = get_token_literal(let->token);
  if (strcmp(literal, "let") != 0) {
    printf("token_literal not 'let'. got=%s\n", literal);
    free(literal);
    return false;
  }
  free(literal);

  if (strcmp(let->name->value, name) != 0) {
    printf("let statement value not '%s'. got=%s\n", name, let->name->value);
    return false;
  }

  char *name_literal = get_token_literal(let->name->token);
  if (strcmp(name_literal, name) != 0) {
    printf("Name not '%s'. got=%s\n", name, name_literal);
    free(name_literal);
    return false;
  }
  free(name_literal);

  return true;
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

bool test_integer_literal(expression_t *expression, int32_t value) {
  if (expression->type != INT_EXP) {
    printf("expression is not integer expression (%d). got=(%d)\n", INT_EXP,
           expression->type);
    return false;
  }

  integer_t *integer = expression->expression.integer;

  if (integer->value != value) {
    printf("integer.Value is not %" PRId32 ". got=%" PRId32 "\n",
           integer->value, value);
    return false;
  }

  char *value_str = NULL;
  asprintf(&value_str, "%" PRId32, value);
  if (strcmp(integer->token->literal, value_str) != 0) {
    printf("integer.token is not %s. got %s\n", value_str,
           integer->token->literal);
    free(value_str);
    return false;
  }
  free(value_str);
  return true;
}

void TestLetStatements() {

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
    return;
  }

  assert((program != NULL) && "parse_program returned NULL");

  if (program->len != 3) {
    printf("program.Statements does not contain 3 statements. got=%lu\n",
           program->len);
    assert(program->len != 3);
  }

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
    test_let_statement(statement, tests[i].expected_identifier);
  }

  program_destroy(&program);
  parser_destroy(&p); /* destroys lexer too */
  puts("Pass: TestLetStatements");
}

void TestReturnStatements() {
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

  assert((program != NULL) && "parse_program returned NULL");

  if (program->len != 3) {
    printf("program.Statements does not contain 3 statements. got=%lu\n",
           program->len);
    assert(program->len != 3);
  }

  for (int i = 0; i < program->len; i++) {
    statement_t *statement = program->statements[i];
    assert((statement->type == RETURN_STATEMENT) &&
           "Statement is not a return statement");

    return_statement_t *return_statement =
        statement->statement.return_statement;

    if (strcmp(return_statement->token->literal, "return") != 0) {
      printf("return statement token literal is not 'return', got=%s\n",
             return_statement->token->literal);
      continue;
    }
  }

  program_destroy(&program);
  parser_destroy(&parser); /* destroys lexer too */
  puts("Pass: TestReturnStatements");
};

void TestIdentifierExpressions() {
  const char *input = "foobar;";

  lexer_t *lexer = lexer_new(input);
  parser_t *parser = parser_new(lexer);
  program_t *program = parser_parse_program(parser);

  if (check_parser_errors(parser)) {
    program_destroy(&program);
    parser_destroy(&parser);
    return;
  }

  assert((program != NULL) && "parse_program returned NULL");

  char *err_msg = NULL;
  asprintf(&err_msg, "program has not enough statements. got=%lu", program->len);
  assert_fail(program->len == 1, &err_msg);

  statement_t *statement = program->statements[0];
  asprintf(&err_msg, "Expected expression statement (%d), got=%d",
           EXPRESSION_STATEMENT, statement->type);
  assert_fail(statement->type == EXPRESSION_STATEMENT, &err_msg);

  expression_statement_t *expression_statement =
      statement->statement.expression_statement;
  expression_t *expression = expression_statement->expression;
  asprintf(&err_msg, "Expected identifier (%d), got=%d", IDENT_EXP,
           expression->type);
  assert_fail(expression->type == IDENT_EXP, &err_msg);

  identifier_t *identifier = expression->expression.identifier;
  asprintf(&err_msg, "ident.Value not %s. got=%s", "foobar", identifier->value);
  assert_fail(strcmp(identifier->value, "foobar") == 0, &err_msg);

  asprintf(&err_msg, "ident token not %s. got=%s", "foobar",
           identifier->token->literal);
  assert_fail(strcmp(identifier->token->literal, "foobar") == 0, &err_msg);

  program_destroy(&program);
  parser_destroy(&parser);
  puts("Pass: TestIdentifierExpressions");
}

void TestIntegerLiteralExpression(void) {

  const char *input = "5;";

  lexer_t *lexer = lexer_new(input);
  parser_t *parser = parser_new(lexer);
  program_t *program = parser_parse_program(parser);

  if (check_parser_errors(parser)) {
    program_destroy(&program);
    parser_destroy(&parser);
    return;
  }

  assert((program != NULL) && "parse_program returned NULL");

  char *err_msg = NULL;
  asprintf(&err_msg, "program has not enough statements. got=%ld", program->len);
  assert_fail(program->len == 1, &err_msg);

  statement_t *statement = program->statements[0];
  asprintf(&err_msg, "Expected expression statement (%d), got=%d",
           EXPRESSION_STATEMENT, statement->type);
  assert_fail(statement->type == EXPRESSION_STATEMENT, &err_msg);

  expression_statement_t *expression_statement =
      statement->statement.expression_statement;
  expression_t *expression = expression_statement->expression;
  asprintf(&err_msg, "Expected integer (%d), got=%d", INT_EXP,
           expression->type);
  assert_fail(expression->type == INT_EXP, &err_msg);

  integer_t *integer = expression->expression.integer;
  asprintf(&err_msg, "int.Value not %" PRId32 ". got=%" PRId32, 5,
           integer->value);
  assert_fail(integer->value == 5, &err_msg);

  asprintf(&err_msg, "int token not %s. got=%s", "5", integer->token->literal);
  assert_fail(integer->value == 5, &err_msg);

  program_destroy(&program);
  parser_destroy(&parser);
  puts("Pass: TestIntegerLiteralExpressions");
}

void TestParsingPrefixExpression(void) {
  typedef struct _prefix_results_t {
    char *input;
    char *operator;
    int32_t integer_value;
  } prefix_results_t;

  prefix_results_t prefix_tests[] = {
      {"!5;", "!", 5},
      /* {"-15;", "-", 15}, */
  };

  size_t tests_len = sizeof(prefix_tests) / sizeof(*prefix_tests);

  for (int i = 0; i < tests_len; i++) {
    lexer_t *lexer = lexer_new(prefix_tests[i].input);
    parser_t *parser = parser_new(lexer);
    program_t *program = parser_parse_program(parser);

    char *err_msg = NULL;
    asprintf(&err_msg,
             "program.statements does not contain %d statements. got=%ld\n", 1,
             program->len);
    assert_fail(program->len == 1, &err_msg);

    statement_t *statement = program->statements[0];
    asprintf(
        &err_msg,
        "program.statements[0] is not expression_statement_t (%d). got=%d\n",
        EXPRESSION_STATEMENT, statement->type);
    assert_fail(statement->type == EXPRESSION_STATEMENT, &err_msg);

    expression_statement_t *expression_statement =
        statement->statement.expression_statement;
    expression_t *expression = expression_statement->expression;
    asprintf(&err_msg, "expression is not a prefix expression (%d). got (%d)\n",
             PREFIX_EXP, expression->type);
    assert_fail(expression->type == PREFIX_EXP, &err_msg);

    prefix_t *prefix = expression->expression.prefix;
    asprintf(&err_msg, "operator is not '%s'. got=%s\n",
             prefix_tests[i].operator, prefix->operator->literal);
    assert_fail(strcmp(prefix->operator->literal, prefix_tests[i].operator) ==
                    0,
                &err_msg);

    if (!test_integer_literal(prefix->operand, prefix_tests[i].integer_value)) {
      program_destroy(&program);
      parser_destroy(&parser);
      return;
    }
    program_destroy(&program);
    parser_destroy(&parser);
  }

  puts("Pass: TestParsingPrefixExpression");
}

void TestParsingInfixExpressions(void) {

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

  size_t tests_len = sizeof(infix_tests) / sizeof(*infix_tests);

  for (int i = 0; i < tests_len; i++) {
    lexer_t *lexer = lexer_new(infix_tests[i].input);
    parser_t *parser = parser_new(lexer);
    program_t *program = parser_parse_program(parser);

    char *err_msg = NULL;
    asprintf(&err_msg,
             "program.statements does not contain %d statements. got=%ld\n", 1,
             program->len);
    assert_fail(program->len == 1, &err_msg);

    statement_t *statement = program->statements[0];
    asprintf(
        &err_msg,
        "program.statements[0] is not expression_statement_t (%d). got=%d\n",
        EXPRESSION_STATEMENT, statement->type);
    assert_fail(statement->type == EXPRESSION_STATEMENT, &err_msg);

    expression_statement_t *expression_statement =
        statement->statement.expression_statement;
    expression_t *expression = expression_statement->expression;
    asprintf(&err_msg, "expression is not a infix expression (%d). got (%d)\n",
             INFIX_EXP, expression->type);
    assert_fail(expression->type == INFIX_EXP, &err_msg);

    infix_t *infix = expression->expression.infix;
    asprintf(&err_msg, "operator is not '%s'. got=%s\n",
             infix_tests[i].operator, infix->operator->literal);
    assert_fail(strcmp(infix->operator->literal, infix_tests[i].operator) == 0,
                &err_msg);

    if (!test_integer_literal(infix->left, infix_tests[i].left_value)) {
      program_destroy(&program);
      parser_destroy(&parser);
      return;
    }

    if (!test_integer_literal(infix->right, infix_tests[i].right_value)) {
      program_destroy(&program);
      parser_destroy(&parser);
      return;
    }
    program_destroy(&program);
    parser_destroy(&parser);
  }

  puts("Pass: TestParsingInfixExpressions");
}

void TestOperatorPrecedenceParsing(void) {

  typedef struct _operator_precedence_tests {
    char *input;
    char *expected;
  } operator_precedence_tests;

  operator_precedence_tests tests[] = {
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
  };

  size_t tests_len = sizeof(tests) / sizeof(*tests);

  for (int i = 0; i < tests_len; i++) {
    lexer_t *lexer = lexer_new(tests[i].input);
    parser_t *parser = parser_new(lexer);
    program_t *program = parser_parse_program(parser);

    char *program_str = program_to_string(program);

    char *err = NULL;
    asprintf(&err, "Expected: %s, got=%s\n", tests[i].expected, program_str);
    assert_fail(strcmp(program_str, tests[i].expected) == 0, &err);
    free(program_str);

    program_destroy(&program);
    parser_destroy(&parser);
  }

  puts("Pass: TestOperatorPrecedenceParsing");
}

int main(void) {

  signal(SIGSEGV, handler);

  TestLetStatements();

  TestReturnStatements();

  TestIdentifierExpressions();

  TestIntegerLiteralExpression();

  TestParsingPrefixExpression();

  TestParsingInfixExpressions();

  TestOperatorPrecedenceParsing();

  return 0;
}
