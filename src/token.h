#ifndef TOKEN_H
#define TOKEN_H

#include "utils.h"

#define KEYWORDS_SIZE 50

typedef enum {
  ILLEGAL_TOKEN,
  EOF_TOKEN,

  /* Identifiers + literals */
  IDENT_TOKEN, /* add, foobar, x, y, .... */
  INT_TOKEN,

  /* Operators */
  ASSIGN_TOKEN,
  PLUS_TOKEN,
  MINUS_TOKEN,
  BANG_TOKEN,
  ASTERISK_TOKEN,
  SLASH_TOKEN,

  LT_TOKEN,
  GT_TOKEN,

  EQ_TOKEN,
  NOT_EQ_TOKEN,

  /* Delimiters */
  COMMA_TOKEN,
  SEMICOLON_TOKEN,

  LPAREN_TOKEN,
  RPAREN_TOKEN,
  LBRACE_TOKEN,
  RBRACE_TOKEN,

  FUNCTION_TOKEN,
  LET_TOKEN,
  TRUE_TOKEN,
  FALSE_TOKEN,
  IF_TOKEN,
  ELSE_TOKEN,
  RETURN_TOKEN,
} TOKEN;

typedef TOKEN TokenType;

struct _token_t {
  TokenType type;
  char *literal;
};

typedef struct _token_t token_t;

char *token_to_str(TokenType t);
token_t *token_new(TokenType type, char ch);
void token_destroy(token_t **tok_p);

TOKEN *keywords_initialize(void);
void keywords_destroy(TOKEN **t_p);
TOKEN keywords_get(TOKEN *keywords, char *str);

#endif
