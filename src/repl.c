#include "repl.h"

void print_parser_errors(parser_t *parser) {
  for (int i = 0; i < parser->errors_len; i++) {
    puts(parser->errors[i]);
  }
}

void start(FILE *in, FILE *out) {

  char *line = NULL;
  size_t len = 0;

  while (true) {
    printf("%s", PROMPT);
    int nbytes = getline(&line, &len, in);
    if (nbytes == -1) {
      /*
       * Detect EOF
       * https://stackoverflow.com/questions/23583480/using-getline-to-get-input-and-check-if-that-is-ctrl-d-in-c
       */
      free(line);
      break;
    }
    if (line == NULL || strlen(line) == 0) {
      break;
    }
    lexer_t *l = lexer_new(line);
    parser_t *parser = parser_new(l);
    assert(parser);
    program_t *program = parser_parse_program(parser);

    if (parser->errors_len != 0) {
      print_parser_errors(parser);
    } else {
      char *program_str = program_to_string(program);
      if (program_str != NULL)
        printf("%s\n", program_str);
      free(program_str);
    }

    program_destroy(&program);
    parser_destroy(&parser);

    free(line);
    line = NULL;
  }
}
