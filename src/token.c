#include "token.h"

ht_t *keywords_initialize(void) {
  ht_t *ht = ht_create(KEYWORDS_SIZE);
  ht_add(ht, "fn", hd_create(HD_INT_DT, (uintptr_t *)FUNCTION_TOKEN));
  ht_add(ht, "let", hd_create(HD_INT_DT, (uintptr_t *)LET_TOKEN));
  ht_add(ht, "true", hd_create(HD_INT_DT, (uintptr_t *)TRUE_TOKEN));
  ht_add(ht, "false", hd_create(HD_INT_DT, (uintptr_t *)FALSE_TOKEN));
  ht_add(ht, "if", hd_create(HD_INT_DT, (uintptr_t *)IF_TOKEN));
  ht_add(ht, "else", hd_create(HD_INT_DT, (uintptr_t *)ELSE_TOKEN));
  ht_add(ht, "return", hd_create(HD_INT_DT, (uintptr_t *)RETURN_TOKEN));
  return ht;
}

TOKEN keywords_get(ht_t *keywords, char *str) {
  assert(keywords);
  return ht_exists(keywords, str) ? ht_get(keywords, str)->num : 0;
}

void keywords_destroy(ht_t **ht_p) {
  assert(ht_p);
  if (*ht_p) {
    ht_t *ht = *ht_p;
    assert(ht);
    ht_destroy(&ht);
    *ht_p = NULL;
  }
}

const char *token_to_str(TokenType t) {
  switch (t) {
  case ILLEGAL_TOKEN:
    return "ILLEGAL_TOKEN";
  case EOF_TOKEN:
    return "EF_TOKEN";
  case IDENT_TOKEN:
    return "IDENT_TOKEN";
  case INT_TOKEN:
    return "INT_TOKEN";
  case ASSIGN_TOKEN:
    return "ASSIGN_TOKEN";
  case PLUS_TOKEN:
    return "PLUS_TOKEN";
  case MINUS_TOKEN:
    return "MINUS_TOKEN";
  case BANG_TOKEN:
    return "BANG_TOKEN";
  case ASTERISK_TOKEN:
    return "ASTERISK_TOKEN";
  case SLASH_TOKEN:
    return "SLASH_TOKEN";
  case LT_TOKEN:
    return "LT_TOKEN";
  case GT_TOKEN:
    return "GT_TOKEN";
  case EQ_TOKEN:
    return "EQ_TOKEN";
  case NOT_EQ_TOKEN:
    return "NOT_EQ_TOKEN";
  case COMMA_TOKEN:
    return "COMMA_TOKEN";
  case SEMICOLON_TOKEN:
    return "SEMICOLON_TOKEN";
  case LPAREN_TOKEN:
    return "LPAREN_TOKEN";
  case RPAREN_TOKEN:
    return "RPAREN_TOKEN";
  case LBRACE_TOKEN:
    return "LBRACE_TOKEN";
  case RBRACE_TOKEN:
    return "RBRACE_TOKEN";
  case FUNCTION_TOKEN:
    return "FUNCTION_TOKEN";
  case LET_TOKEN:
    return "LET_TOKEN";
  case TRUE_TOKEN:
    return "TRUE_TOKEN";
  case FALSE_TOKEN:
    return "FALSE_TOKEN";
  case IF_TOKEN:
    return "IF_TOKEN";
  case ELSE_TOKEN:
    return "ELSE_TOKEN";
  case RETURN_TOKEN:
    return "RETURN_TOKEN";
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
