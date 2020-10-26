#ifndef TOKEN_H
#define TOKEN_H

#include "utils.h"

typedef enum {
  ILLEGAL,
  EF,

  /* Identifiers + literals */
  IDENT,                        /* add, foobar, x, y, .... */
  INT,

  /* Operators */
  ASSIGN,
  PLUS,

  /* Delimiters */
  COMMA,
  SEMICOLON,

  LPAREN,
  RPAREN,
  LBRACE,
  RBRACE,

  FUNCTION,
  LET
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


#endif
