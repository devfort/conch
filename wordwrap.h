#ifndef _WORDWRAP_H
#define _WORDWRAP_H

typedef struct token_s {
  int y;
  int x;
  int length;
  char *word;
} token_s;

typedef struct wordwrap_s {
  int width;
  char *content;
  size_t _content_len;
  token_s _token;
} wordwrap_s;

void init_wordwrap(wordwrap_s *, char *, int);
token_s *wordwrap(wordwrap_s *);

#endif /* _WORDWRAP_H */
