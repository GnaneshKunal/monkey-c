#include "../src/lexer.h"
#include <check.h>

START_TEST(test_next_token_loop) {

  char *input = "let five = 5; \
let ten = 10;                    \
let add = fn(x, y) {             \
  x + y;                         \
};                               \
let result = add(five, ten);     \
!-/*5;                           \
5 < 10 > 5;                      \
if (5 < 10) {                    \
  return true;                   \
} else {                         \
  return false;                  \
}                                \
10 == 10;                        \
10 != 9;                        \
";

  typedef struct {
    TokenType expected_type;
    char *expected_literal;
  } test_token_t;

  test_token_t tests[] = {
      {LET_TOKEN, "let"},      {IDENT_TOKEN, "five"},    {ASSIGN_TOKEN, "="},
      {INT_TOKEN, "5"},        {SEMICOLON_TOKEN, ";"},   {LET_TOKEN, "let"},
      {IDENT_TOKEN, "ten"},    {ASSIGN_TOKEN, "="},      {INT_TOKEN, "10"},
      {SEMICOLON_TOKEN, ";"},  {LET_TOKEN, "let"},       {IDENT_TOKEN, "add"},
      {ASSIGN_TOKEN, "="},     {FUNCTION_TOKEN, "fn"},   {LPAREN_TOKEN, "("},
      {IDENT_TOKEN, "x"},      {COMMA_TOKEN, ","},       {IDENT_TOKEN, "y"},
      {RPAREN_TOKEN, ")"},     {LBRACE_TOKEN, "{"},      {IDENT_TOKEN, "x"},
      {PLUS_TOKEN, "+"},       {IDENT_TOKEN, "y"},       {SEMICOLON_TOKEN, ";"},
      {RBRACE_TOKEN, "}"},     {SEMICOLON_TOKEN, ";"},   {LET_TOKEN, "let"},
      {IDENT_TOKEN, "result"}, {ASSIGN_TOKEN, "="},      {IDENT_TOKEN, "add"},
      {LPAREN_TOKEN, "("},     {IDENT_TOKEN, "five"},    {COMMA_TOKEN, ","},
      {IDENT_TOKEN, "ten"},    {RPAREN_TOKEN, ")"},      {SEMICOLON_TOKEN, ";"},
      {BANG_TOKEN, "!"},       {MINUS_TOKEN, "-"},       {SLASH_TOKEN, "/"},
      {ASTERISK_TOKEN, "*"},   {INT_TOKEN, "5"},         {SEMICOLON_TOKEN, ";"},
      {INT_TOKEN, "5"},        {LT_TOKEN, "<"},          {INT_TOKEN, "10"},
      {GT_TOKEN, ">"},         {INT_TOKEN, "5"},         {SEMICOLON_TOKEN, ";"},
      {IF_TOKEN, "if"},        {LPAREN_TOKEN, "("},      {INT_TOKEN, "5"},
      {LT_TOKEN, "<"},         {INT_TOKEN, "10"},        {RPAREN_TOKEN, ")"},
      {LBRACE_TOKEN, "{"},     {RETURN_TOKEN, "return"}, {TRUE_TOKEN, "true"},
      {SEMICOLON_TOKEN, ";"},  {RBRACE_TOKEN, "}"},      {ELSE_TOKEN, "else"},
      {LBRACE_TOKEN, "{"},     {RETURN_TOKEN, "return"}, {FALSE_TOKEN, "false"},
      {SEMICOLON_TOKEN, ";"},  {RBRACE_TOKEN, "}"},      {INT_TOKEN, "10"},
      {EQ_TOKEN, "=="},        {INT_TOKEN, "10"},        {SEMICOLON_TOKEN, ";"},
      {INT_TOKEN, "10"},       {NOT_EQ_TOKEN, "!="},     {INT_TOKEN, "9"},
      {SEMICOLON_TOKEN, ";"},  {EOF_TOKEN, ""}};

  lexer_t *lexer = lexer_new(input);

  size_t tests_size = sizeof(tests) / sizeof(*tests);
  for (int i = 0; i < tests_size; i++) {
    token_t *tok = lexer_next_token(lexer);
    TokenType expected_type = tests[i].expected_type;

    const char *expected_type_str = token_to_str(expected_type);
    const char *actual_type_str = token_to_str(tok->type);
    ck_assert_msg(strcmp(expected_type_str, actual_type_str) == 0,
                  "Expected type: %s, Got: %s\n", expected_type_str,
                  actual_type_str);

    ck_assert_msg(strcmp(tok->literal, tests[i].expected_literal) == 0,
                  "Expected Literal: %s, Got: %s\n", tests[i].expected_literal,
                  tok->literal);
    token_destroy(&tok);
  }
  lexer_destroy(&lexer);
}
END_TEST

Suite *lexer_suite(void) {
  Suite *s;
  TCase *tc_core;

  s = suite_create("Lexer");
  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_next_token_loop);

  suite_add_tcase(s, tc_core);

  return s;
}

int main(void) {

  int number_failed;
  Suite *s;
  SRunner *sr;

  s = lexer_suite();
  sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
