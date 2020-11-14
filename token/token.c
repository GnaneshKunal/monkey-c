#include "token.h"

TOKEN *keywords_initialize(void) {
  TOKEN *token_table = calloc(sizeof(TOKEN), KEYWORDS_SIZE);
  assert(token_table);
  token_table[gnu_hash((const uint8_t *)"fn") % KEYWORDS_SIZE] = FUNCTION_TOKEN;
  token_table[gnu_hash((const uint8_t *)"let") % KEYWORDS_SIZE] = LET_TOKEN;
  token_table[gnu_hash((const uint8_t *)"true") % KEYWORDS_SIZE] = TRUE_TOKEN;
  token_table[gnu_hash((const uint8_t *)"false") % KEYWORDS_SIZE] = FALSE_TOKEN;
  token_table[gnu_hash((const uint8_t *)"if") % KEYWORDS_SIZE] = IF_TOKEN;
  token_table[gnu_hash((const uint8_t *)"else") % KEYWORDS_SIZE] = ELSE_TOKEN;
  token_table[gnu_hash((const uint8_t *)"return") % KEYWORDS_SIZE] = RETURN_TOKEN;

  return token_table;
}

TOKEN keywords_get(TOKEN *keywords, char *str) {
  assert(keywords);
  return keywords[gnu_hash((const uint8_t *)str) % KEYWORDS_SIZE];
}

void keywords_destroy(TOKEN **t_p) {
  assert(t_p);
  if (*t_p) {
    TOKEN *t = *t_p;
    assert(t);
    free(t);
    *t_p = NULL;
  }
}

char *token_to_str(TokenType t) {
  switch (t) {
  case ILLEGAL_TOKEN:
    return strdup("ILLEGAL_TOKEN");
  case EOF_TOKEN:
    return strdup("EF_TOKEN");
  case IDENT_TOKEN:
    return strdup("IDENT_TOKEN");
  case INT_TOKEN:
    return strdup("INT_TOKEN");
  case ASSIGN_TOKEN:
    return strdup("ASSIGN_TOKEN");
  case PLUS_TOKEN:
    return strdup("PLUS_TOKEN");
  case MINUS_TOKEN:
    return strdup("MINUS_TOKEN");
  case BANG_TOKEN:
    return strdup("BANG_TOKEN");
  case ASTERISK_TOKEN:
    return strdup("ASTERISK_TOKEN");
  case SLASH_TOKEN:
    return strdup("SLASH_TOKEN");
  case LT_TOKEN:
    return strdup("LT_TOKEN");
  case GT_TOKEN:
    return strdup("GT_TOKEN");
  case EQ_TOKEN:
    return strdup("EQ_TOKEN");
  case NOT_EQ_TOKEN:
    return strdup("NOT_EQ_TOKEN");
  case COMMA_TOKEN:
    return strdup("COMMA_TOKEN");
  case SEMICOLON_TOKEN:
    return strdup("SEMICOLON_TOKEN");
  case LPAREN_TOKEN:
    return strdup("LPAREN_TOKEN");
  case RPAREN_TOKEN:
    return strdup("RPAREN_TOKEN");
  case LBRACE_TOKEN:
    return strdup("LBRACE_TOKEN");
  case RBRACE_TOKEN:
    return strdup("RBRACE_TOKEN");
  case FUNCTION_TOKEN:
    return strdup("FUNCTION_TOKEN");
  case LET_TOKEN:
    return strdup("LET_TOKEN");
  case TRUE_TOKEN:
    return strdup("TRUE_TOKEN");
  case FALSE_TOKEN:
    return strdup("FALSE_TOKEN");
  case IF_TOKEN:
    return strdup("IF_TOKEN");
  case ELSE_TOKEN:
    return strdup("ELSE_TOKEN");
  case RETURN_TOKEN:
    return strdup("RETURN_TOKEN");
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
