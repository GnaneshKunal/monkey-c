#include "lexer.h"

struct _lexer_t {
  char *input;
  uint32_t position; /* current position in input (points to current char) */
  uint32_t
      readPosition; /* current reading position in input (after current char) */
  char ch;          /* current char under examination */
  TOKEN *keywords;
};

bool is_letter(char ch) {
  return 'a' <= ch && ch <= 'z' || 'A' <= ch && ch <= 'Z' || ch == '_';
}

bool is_digit(char ch) {
  return '0' <= ch && ch <= '9';
}

TOKEN lexer_lookup_ident(lexer_t *l, char *ident) {
  /*
   * Lookup the the keywords dict to check whether `ident` is a
   * keyword. If not, we treat it as an `IDENT`.
   */
  assert(ident);
  TOKEN tok = keywords_get(l->keywords, ident);
  return tok != ILLEGAL ? tok : IDENT;
}

void lexer_skip_whitespace(lexer_t *l) {
  while (l->ch == ' ' || l->ch == '\t' || l->ch == '\n' || l->ch == '\r') {
    lexer_read_char(l);
  }
}

lexer_t *lexer_new(char *input) {
  assert(input);

  lexer_t *l = malloc(sizeof(lexer_t));
  l->position = 0;
  l->readPosition = 0;
  l->ch = 0;
  l->input = strdup(input);
  l->keywords = keywords_initialize();

  lexer_read_char(l);

  return l;
}

void lexer_destroy(lexer_t **l_p) {
  assert(l_p);
  if (*l_p) {
    lexer_t *l = *l_p;
    keywords_destroy(&l->keywords);
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

char *lexer_read_identifier(lexer_t *l) {
  assert(l);
  assert(l->input);
  char var[255];
  int i = 0;
  while (is_letter(l->ch)) {
    if (i == 255) {
      return NULL;
    }
    var[i++] = l->ch;
    lexer_read_char(l);
  }
  var[i] = '\0';
  return strdup(var);
}

char *lexer_read_number(lexer_t *l) {
  assert(l);
  assert(l->input);

  char number[50];
  int i = 0;
  while (is_digit(l->ch)) {
    if (i == 50) {
      return NULL;
    }
    number[i++] = l->ch;
    lexer_read_char(l);
  }
  number[i] = '\0';
  return strdup(number);
}

token_t *lexer_next_token(lexer_t *l) {

  token_t *tok = NULL;

  lexer_skip_whitespace(l);

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
  default:
    if (is_letter(l->ch)) {
      tok = malloc(sizeof(token_t));
      tok->literal = lexer_read_identifier(l);
      tok->type = lexer_lookup_ident(l, tok->literal);
      /*
       * we call `lexer_read_char` in `lexer_read_identifier` so early
       * exit is required here.
       */
      return tok;
    } else if(is_digit(l->ch)) {
      tok = malloc(sizeof(token_t));
      tok->literal = lexer_read_number(l);
      tok->type = INT;
      /*
       * we call `lexer_read_char` in `lexer_read_identifier` so early
       * exit is required here.
       */
      return tok;
    } else {
      tok = malloc(sizeof(token_t));
      tok->literal = "";
      tok->type = ILLEGAL;
    }
  }
  lexer_read_char(l);
  return tok;
}
