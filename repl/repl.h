#ifndef REPL_H
#define REPL_H

#include "../lexer/lexer.h"
#include "../token/token.h"
#include "../utils/utils.h"

const char *PROMPT = ">> ";

void start(FILE *in, FILE *out);

#endif
