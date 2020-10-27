#include "lexer.h"

struct _lexer_t {
  char *input;
  uint32_t position; /* current position in input (points to current char) */
  uint32_t
      readPosition; /* current reading position in input (after current char) */
  char ch;          /* current char under examination */
};

lexer_t *lexer_new(char *input) {
  assert(input);

  lexer_t *l = malloc(sizeof(lexer_t));
  l->position = 0;
  l->readPosition = 0;
  l->ch = 0;
  l->input = strdup(input);

  lexer_read_char(l);

  return l;
}

void lexer_destroy(lexer_t **l_p) {
  assert(l_p);
  if (*l_p) {
    lexer_t *l = *l_p;
    free(l->input);
    free(l);
    *l_p = NULL;
  }
}

void lexer_read_char(lexer_t *l) {
  assert(l);
  assert(l->input);
  if (l->readPosition >= strlen(l->input)) {
    l->ch = 0;
  } else {
    l->ch = l->input[l->readPosition];
  }
  l->position = l->readPosition++;
}

token_t *lexer_next_token(lexer_t *l) {
  token_t *tok = NULL;

  switch (l->ch) {
  case '=':
    tok = token_new(ASSIGN, l->ch);
    break;
  case ';':
    tok = token_new(SEMICOLON, l->ch);
    break;
  case '(':
    tok = token_new(LPAREN, l->ch);
    break;
  case ')':
    tok = token_new(RPAREN, l->ch);
    break;
  case ',':
    tok = token_new(COMMA, l->ch);
    break;
  case '+':
    tok = token_new(PLUS, l->ch);
    break;
  case '{':
    tok = token_new(LBRACE, l->ch);
    break;
  case '}':
    tok = token_new(RBRACE, l->ch);
    break;
  case 0:
    tok = malloc(sizeof(token_t));
    tok->literal = "";
    tok->type = ILLEGAL;
  }
  lexer_read_char(l);
  return tok;
}
