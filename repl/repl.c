#include "repl.h"

void start(FILE *in, FILE *out) {

  char *line = NULL;
  size_t len = 0;

  while (true) {
    printf(PROMPT);
    getline(&line, &len, in);
    if (line == NULL || strlen(line) == 0) {
      return;
    }
    lexer_t *l = lexer_new(line);
    token_t *tok;
    while (true) {
      tok = lexer_next_token(l);
      assert(tok);
      if (tok->type == EOF_TOKEN) {
        token_destroy(&tok);
        break;
      }
      char *tok_type = token_to_str(tok->type);
      printf("{type: %s, literal: %s}\n", tok_type, tok->literal);
      free(tok_type);
      token_destroy(&tok);
    }
    lexer_destroy(&l);
    free(line);
    line = NULL;
  }
}

int main(void) {

  char user[HOST_NAME_MAX];
  gethostname(user, HOST_NAME_MAX);
  printf("Hello %s! This is the Monkey programming language!\n", user);
  printf("Feel free to type in commands\n");

  start(stdin, stdout);

  return 0;
}
