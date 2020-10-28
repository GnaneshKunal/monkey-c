#include "lexer.h"

void TestNextToken(void) {

  char input[] = "let five = 5; \
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

  test_token_t tests[] = {{LET, "let"},     {IDENT, "five"},
                          {ASSIGN, "="},    {INT, "5"},
                          {SEMICOLON, ";"}, {LET, "let"},
                          {IDENT, "ten"},   {ASSIGN, "="},
                          {INT, "10"},      {SEMICOLON, ";"},
                          {LET, "let"},     {IDENT, "add"},
                          {ASSIGN, "="},    {FUNCTION, "fn"},
                          {LPAREN, "("},    {IDENT, "x"},
                          {COMMA, ","},     {IDENT, "y"},
                          {RPAREN, ")"},    {LBRACE, "{"},
                          {IDENT, "x"},     {PLUS, "+"},
                          {IDENT, "y"},     {SEMICOLON, ";"},
                          {RBRACE, "}"},    {SEMICOLON, ";"},
                          {LET, "let"},     {IDENT, "result"},
                          {ASSIGN, "="},    {IDENT, "add"},
                          {LPAREN, "("},    {IDENT, "five"},
                          {COMMA, ","},     {IDENT, "ten"},
                          {RPAREN, ")"},    {SEMICOLON, ";"},
                          {BANG, "!"},      {MINUS, "-"},
                          {SLASH, "/"},     {ASTERISK, "*"},
                          {INT, "5"},       {SEMICOLON, ";"},
                          {INT, "5"},       {LT, "<"},
                          {INT, "10"},      {GT, ">"},
                          {INT, "5"},       {SEMICOLON, ";"},
                          {IF, "if"},       {LPAREN, "("},
                          {INT, "5"},       {LT, "<"},
                          {INT, "10"},      {RPAREN, ")"},
                          {LBRACE, "{"},    {RETURN, "return"},
                          {TRUE, "true"},   {SEMICOLON, ";"},
                          {RBRACE, "}"},    {ELSE, "else"},
                          {LBRACE, "{"},    {RETURN, "return"},
                          {FALSE, "false"}, {SEMICOLON, ";"},
                          {RBRACE, "}"},    {INT, "10"},
                          {EQ, "=="},       {INT, "10"},
                          {SEMICOLON, ";"}, {INT, "10"},
                          {NOT_EQ, "!="},   {INT, "9"},
                          {SEMICOLON, ";"}, {EF, ""}};
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
