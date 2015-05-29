#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "strutils.h"
#include "explode.h"

char *strclone(const char *c) {
  if (c == NULL) {
    return NULL;
  }

  char *target = malloc(strlen(c) + 1);

  if (target == NULL) {
    fatal_error("strclone: could not alloc");
  }

  strcpy(target, c);
  return target;
}

char *strcopycat(const char *c, const char *d) {
  assert(c != NULL);
  assert(d != NULL);

  int n = strlen(c);
  char *target = malloc(n + strlen(d) + 1);

  if (target == NULL) {
    fatal_error("strcopycat: could not alloc");
  }

  strcpy(target, c);
  strcpy(target + n, d);
  return target;
}

char *stralleycat(int count, char **strs) {
  int i = 0, size = 0;
  int idx = 0;
  for (i = 0; i < count; i++) {
    size += strlen(strs[i]) + 1;
  }

  char *result = malloc(size * sizeof(char));
  if (result == NULL) {
    fatal_error("stralleycat: could not alloc");
  }

  for (i = 0; i < count; i++) {
    strcpy(result + idx, strs[i]);
    idx += strlen(strs[i]);
    result[idx] = ' ';
    idx += 1;
  }
  result[idx - 1] = 0;
  return result;
}

char *strcopytrunc(char const *const src, unsigned int width) {
  char *result = malloc((width + 1) * sizeof(char));
  if (result == NULL) {
    fatal_error("strcopytrunc: could not alloc");
  }

  strncpy(result, src, width);
  result[width] = '\0';
  return result;
}

char *expand_home(const char *path) {
  char *tilde = strchr(path, '~');
  if (tilde != NULL) {
    int index = tilde - path;
    char *home = getenv("HOME");
    char *result = calloc(sizeof(char), strlen(path) + strlen(home));
    strncat(result, path, index);
    strcat(result, home);
    strcat(result, tilde + 1);
    return result;
  } else {
    return strclone(path);
  }
  return tilde;
}

int count_lines_and_find_length_of_longest(const char *string,
                                           int *out_longest_line) {
  *out_longest_line = 0;
  int lines = 0;

  char const *start_of_line = string, *end_of_line;

  end_of_line = strchr(string, '\n');
  while (start_of_line != NULL && end_of_line != NULL) {
    int this_line_len = end_of_line - start_of_line;
    if (this_line_len > *out_longest_line) {
      *out_longest_line = this_line_len;
    }
    lines++;

    // Skip over the new line
    start_of_line = end_of_line + 1;
    end_of_line = strchr(start_of_line, '\n');
  }

  // Deal with the last line, or an input with zero new lines in
  int this_line_len = strlen(start_of_line);
  if (this_line_len > *out_longest_line) {
    *out_longest_line = this_line_len;
  }

  return lines + 1;
}

/*
 * Copy the string represented by the pointer range [`l`, `r`).
 *
 * That is, if `l` and `r` are pointers into a longer string thus:
 *
 *     l    r
 *     |    |
 *    "Hello, world."
 *
 * the returned string will be (NUL-terminated) "Hello".
 */
static char *strcpyrange(char *l, char *r) {
  char *dst = malloc((r - l + 1) * sizeof(char));
  if (dst == NULL) {
    fatal_error("strcpyrange: could not allocate");
  }
  strncpy(dst, l, r - l);
  dst[r - l] = '\0';
  return dst;
}

/*
 * Append a line (a copy of the string between `l` and `r`) to the array
 * `lines`, reallocating if necessary. `nlines` is the current size of the
 * `lines` array, and will be incremented.
 */
static char **appendline(char **lines, unsigned int *nlines, char *l, char *r) {
  lines = realloc(lines, ((*nlines) + 1) * sizeof(char *));
  if (lines == NULL) {
    fatal_error("appendline: could not allocate %u lines", (*nlines) + 1);
  }
  lines[(*nlines)++] = strcpyrange(l, r);

  return lines;
}

/*
 * Given a NUL-terminated string `text`, and a wrap width `width`, return an
 * array of lines representing the wrapped string.
 *
 * Writes the number of wrapped lines to the pointer `nlines`.
 */
char **wrap_lines(char const *const text, int width, unsigned int *nlines) {
  char *cursl = (char *)text;
  char *cursr = (char *)text;
  char *linestart = (char *)text;
  char *lastbreak = NULL;
  char **result = NULL;

  *nlines = 0;

  // NULL text, empty text, or zero/negative width: we return one empty line.
  if (text == NULL || strlen(text) == 0 || width <= 0) {
    result = malloc(sizeof(char *));
    char *s = calloc(1, sizeof(char));
    result[(*nlines)++] = s;
    return result;
  }

  while (*cursr != '\0') {
    // When we encounter a newline, we break the current line.
    if (*cursr == '\n') {
      result = appendline(result, nlines, cursl, cursr);
      cursr++;
      linestart = cursl = cursr;
      lastbreak = NULL;
      continue;
    }

    // Preserve any whitespace at the start of a line
    if (linestart) {
      if (isspace(*cursr)) {
        cursr++;
        continue;
      } else {
        linestart = NULL;
      }
    }

    // If we just broke the line, skip to a non-space character
    if (cursl == cursr && isspace(*cursr)) {
      cursl++;
      cursr++;
      continue;
    }

    // Otherwise, when we encounter whitespace, keep a reference to it
    if (isspace(*cursr)) {
      lastbreak = cursr;
    }

    if (cursr - cursl == width) {
      if (lastbreak == NULL) {
        result = appendline(result, nlines, cursl, cursr);
        cursl = cursr;
        continue;
      }
      result = appendline(result, nlines, cursl, lastbreak);
      cursl = cursr = lastbreak + 1;
      lastbreak = NULL;
      continue;
    }

    cursr++;
  }

  result = appendline(result, nlines, cursl, cursr);
  return result;
}

void wrap_lines_free(char **lines, unsigned int nlines) {
  if (lines == NULL) {
    return;
  }

  for (unsigned int i = 0; i < nlines; i++) {
    free(lines[i]);
  }

  free(lines);
}
