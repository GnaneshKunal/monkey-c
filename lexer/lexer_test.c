#include "lexer.h"

void TestNextToken(void) {

  char input[] = "=+(){},;";

  typedef struct {
    TokenType expected_type;
    char *expected_literal;
  } test_token_t;

  test_token_t tests[] = {
      {ASSIGN, "="}, {PLUS, "+"},   {LPAREN, "("}, {RPAREN, ")"},
      {LBRACE, "{"}, {RBRACE, "}"}, {COMMA, ","},  {SEMICOLON, ";"},
  };
  size_t tests_size = sizeof(tests) / sizeof(*tests);

  lexer_t *l = lexer_new(input);

  for (int i = 0; i < tests_size; i++) {
    token_t *tok = lexer_next_token(l);

    TokenType expected_type = tests[i].expected_type;

    char message[50];

    char *expected_type_str = token_to_str(expected_type);
    char *actual_type_str = token_to_str(tok->type);
    sprintf(message, "Expected type: %s, Got: %s\n", expected_type_str,
            actual_type_str);
    assert((tok->type == expected_type) && message);
    free(expected_type_str);
    free(actual_type_str);

    sprintf(message, "Expected Literal: %s, Got: %s\n",
            tests[i].expected_literal, tok->literal);
    assert((strcmp(tok->literal, tests[i].expected_literal) == 0) && message);

    token_destroy(&tok);
  }
  lexer_destroy(&l);

  puts("PASS: TestNextToken");
}

int main(void) {

  signal(SIGSEGV, handler);

  TestNextToken();

  return 0;
}
