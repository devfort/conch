#include <ctype.h>
#include <string.h>
#include "wordwrap.h"

void init_wordwrap(wordwrap_s *wrap, char *content, int width) {
  wrap->width = width;
  wrap->content = content;
  wrap->_content_len = strlen(content);

  wrap->_token.y = 0;
  wrap->_token.x = 0;
  wrap->_token.length = 0;
  wrap->_token.word = content;
}

token_s *wordwrap(wordwrap_s *wrap) {
  char *content_end = wrap->content + wrap->_content_len;
  char *start = wrap->_token.word + wrap->_token.length;
  char *p, *token_start;

  if (start >= content_end) {
    return NULL;
  }

  wrap->_token.x += wrap->_token.length;

  // skip leading spaces
  for (p = start; p < content_end && isspace(*p); ++p)
    ;

  wrap->_token.x += (p - start);

  // find next token
  for (token_start = p; p < content_end && !isspace(*p); ++p)
    ;

  size_t token_length = p - token_start;

  if (wrap->_token.x + token_length >= wrap->width) {
    wrap->_token.x = 0;
    wrap->_token.y += 1;
  }

  wrap->_token.word = token_start;
  wrap->_token.length = token_length;

  return &(wrap->_token);
}
