#include "lexer.h"

struct _lexer_t {
  const char *input;
  uint32_t position; /* current position in input (points to current char) */
  uint32_t read_position; /* current reading position in input (after current
                             char) */
  char ch;                /* current char under examination */
  TOKEN *keywords;
};

bool is_letter(char ch) {
  return 'a' <= ch && ch <= 'z' || 'A' <= ch && ch <= 'Z' || ch == '_';
}

bool is_digit(char ch) { return '0' <= ch && ch <= '9'; }

TOKEN lexer_lookup_ident(lexer_t *l, char *ident) {
  /*
   * Lookup the the keywords dict to check whether `ident` is a
   * keyword. If not, we treat it as an `IDENT`.
   */
  assert(ident);
  TOKEN tok = keywords_get(l->keywords, ident);
  return tok != ILLEGAL_TOKEN ? tok : IDENT_TOKEN;
}

void lexer_skip_whitespace(lexer_t *l) {
  while (l->ch == ' ' || l->ch == '\t' || l->ch == '\n' || l->ch == '\r') {
    lexer_read_char(l);
  }
}

lexer_t *lexer_new(const char *input) {
  assert(input);

  lexer_t *l = malloc(sizeof(lexer_t));
  l->position = 0;
  l->read_position = 0;
  l->ch = 0;
  l->input = input;
  l->keywords = keywords_initialize();

  lexer_read_char(l);

  return l;
}

void lexer_destroy(lexer_t **l_p) {
  assert(l_p);
  if (*l_p) {
    lexer_t *l = *l_p;
    keywords_destroy(&l->keywords);
    free(l);
    *l_p = NULL;
  }
}

void lexer_read_char(lexer_t *l) {
  assert(l);
  assert(l->input);
  if (l->read_position >= strlen(l->input)) {
    l->ch = 0;
  } else {
    l->ch = l->input[l->read_position];
  }
  l->position = l->read_position++;
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

char lexer_peek_char(lexer_t *l) {
  if (l->read_position >= strlen(l->input)) {
    return '\0';
  } else {
    return l->input[l->read_position];
  }
}

token_t *lexer_next_token(lexer_t *l) {

  token_t *tok = NULL;

  lexer_skip_whitespace(l);

  switch (l->ch) {
  case '=':
    if (lexer_peek_char(l) == '=') {
      tok = malloc(sizeof(token_t));
      tok->type = EQ_TOKEN;
      tok->literal = strdup("==");
      lexer_read_char(l);
    } else {
      tok = token_new(ASSIGN_TOKEN, l->ch);
    }
    break;
  case '(':
    tok = token_new(LPAREN_TOKEN, l->ch);
    break;
  case ')':
    tok = token_new(RPAREN_TOKEN, l->ch);
    break;
  case '+':
    tok = token_new(PLUS_TOKEN, l->ch);
    break;
  case '-':
    tok = token_new(MINUS_TOKEN, l->ch);
    break;
  case '!':
    if (lexer_peek_char(l) == '=') {
      tok = malloc(sizeof(token_t));
      tok->type = NOT_EQ_TOKEN;
      tok->literal = strdup("!=");
      lexer_read_char(l);
    } else {
      tok = token_new(BANG_TOKEN, l->ch);
    }
    break;
  case '/':
    tok = token_new(SLASH_TOKEN, l->ch);
    break;
  case '*':
    tok = token_new(ASTERISK_TOKEN, l->ch);
    break;
  case '<':
    tok = token_new(LT_TOKEN, l->ch);
    break;
  case '>':
    tok = token_new(GT_TOKEN, l->ch);
    break;
  case ',':
    tok = token_new(COMMA_TOKEN, l->ch);
    break;
  case ';':
    tok = token_new(SEMICOLON_TOKEN, l->ch);
    break;
  case '{':
    tok = token_new(LBRACE_TOKEN, l->ch);
    break;
  case '}':
    tok = token_new(RBRACE_TOKEN, l->ch);
    break;
  case 0:
    tok = malloc(sizeof(token_t));
    tok->literal = strdup("");
    tok->type = EOF_TOKEN;
    break;
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
    } else if (is_digit(l->ch)) {
      tok = malloc(sizeof(token_t));
      tok->literal = lexer_read_number(l);
      tok->type = INT_TOKEN;
      /*
       * we call `lexer_read_char` in `lexer_read_identifier` so early
       * exit is required here.
       */
      return tok;
    } else {
      tok = malloc(sizeof(token_t));
      tok->literal = malloc(sizeof(char));
      tok->literal[0] = '\0';
      tok->type = ILLEGAL_TOKEN;
    }
  }
  lexer_read_char(l);
  return tok;
}
