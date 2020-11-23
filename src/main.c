#include "repl.h"

int main(void) {

  char user[HOST_NAME_MAX];
  gethostname(user, HOST_NAME_MAX);
  printf("Hello %s! This is the Monkey programming language!\n", user);
  printf("Feel free to type in commands\n");

  start(stdin, stdout);

  return 0;
}
