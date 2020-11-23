#ifndef REPL_H
#define REPL_H

#include "lexer.h"
#include "token.h"
#include "utils.h"

const char *PROMPT = ">> ";

void start(FILE *in, FILE *out);

#endif
