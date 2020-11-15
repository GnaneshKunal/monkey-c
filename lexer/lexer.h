#ifndef LEXER_H
#define LEXER_H

#include "../token/token.h"
#include "../utils/utils.h"

typedef struct _lexer_t lexer_t;

lexer_t *lexer_new(const char *input);
void lexer_destroy(lexer_t **l_p);
void lexer_read_char(lexer_t *l);
char lexer_peek_char(lexer_t *l);
char *lexer_read_number(lexer_t *l);
char *lexer_read_identifier(lexer_t *l);
token_t *lexer_next_token(lexer_t *l);

bool is_letter(char ch);
bool is_digit(char ch);
TOKEN lexer_lookup_ident(lexer_t *l, char *ident);
void lexer_skip_whitespace(lexer_t *l);

#endif
