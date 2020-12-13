#include "../src/parser.h"

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
