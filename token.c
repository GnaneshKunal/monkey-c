#include "token.h"

char *token_to_str(TokenType t) {
  switch(t) {
  case ILLEGAL:
    return strdup("ILLEGAL");
  case EF:
    return strdup("EF");
  case IDENT:
    return strdup("IDENT");
  case INT:
    return strdup("INT");
  case ASSIGN:
    return strdup("ASSIGN");
  case PLUS:
    return strdup("PLUS");
  case COMMA:
    return strdup("COMMA");
  case SEMICOLON:
    return strdup("SEMICOLON");
  case LPAREN:
    return strdup("LPAREN");
  case RPAREN:
    return strdup("RPAREN");
  case LBRACE:
    return strdup("LBRACE");
  case RBRACE:
    return strdup("RBRACE");
  case FUNCTION:
    return strdup("FUNCTION");
  case LET:
    return strdup("LET");
  }

  return NULL;
}


token_t *token_new(TokenType type, char ch) {
  token_t *tok = malloc(sizeof(token_t));
  assert(tok);
  char *literal = malloc(sizeof(char) * 2);
  assert(literal);
  literal[0] = ch;
  literal[1] = '\0';
  tok->type = type;
  tok->literal = literal;
  return tok;
}

void token_destroy(token_t **tok_p) {
  assert(tok_p);
  if (*tok_p) {
    token_t *tok = *tok_p;
    assert(tok);
    assert(tok->literal);
    free(tok->literal);
    free(tok);
    *tok_p = NULL;
  }
}
