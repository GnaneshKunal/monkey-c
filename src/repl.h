#ifndef REPL_H
#define REPL_H

#include "lexer.h"
#include "token.h"
#include "parser.h"
#include "evaluator.h"
#include "utils.h"

#define PROMPT ">> "

void start(FILE *in, FILE *out);

#endif
