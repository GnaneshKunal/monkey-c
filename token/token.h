#ifndef TOKEN_H
#define TOKEN_H

#include "../utils/utils.h"

#define KEYWORDS_SIZE 50

typedef enum {
  ILLEGAL,
  EF,

  /* Identifiers + literals */
  IDENT, /* add, foobar, x, y, .... */
  INT,

  /* Operators */
  ASSIGN,
  PLUS,
  MINUS,
  BANG,
  ASTERISK,
  SLASH,

  LT,
  GT,

  EQ,
  NOT_EQ,

  /* Delimiters */
  COMMA,
  SEMICOLON,

  LPAREN,
  RPAREN,
  LBRACE,
  RBRACE,

  FUNCTION,
  LET,
  TRUE,
  FALSE,
  IF,
  ELSE,
  RETURN,
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
