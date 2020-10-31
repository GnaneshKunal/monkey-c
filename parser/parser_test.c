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

void TestLetStatements() {

  char input[] = "                              \
let x = 5;                                      \
let y = 10;                                     \
let foobar = 838383;                            \
";

  lexer_t *l = lexer_new(input);
  parser_t *p = parser_new(l);

  program_t *program = parser_parse_program(p);

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
  puts("Pass TestLetStatements");
}

int main(void) {

  signal(SIGSEGV, handler);

  TestLetStatements();

  return 0;
}
