#ifndef LEXER_H
#define LEXER_H

#include "utils.h"
#include "token.h"

typedef struct _lexer_t lexer_t;

lexer_t *lexer_new(char *input);
void lexer_destroy(lexer_t **l_p);
void lexer_read_char(lexer_t *l);
token_t *lexer_next_token(lexer_t *l);

#endif
