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

char **wrap_lines(char *text, int max_line_length, unsigned int *nout) {
  char **lines = malloc(1024 * sizeof(char *));
  int lines_required = 0;
  char *start_of_line, *remaining_content;
  start_of_line = remaining_content = text;

  if (NULL == text || 0 == strlen(text) || max_line_length <= 0) {
    char *empty_string = calloc(1, sizeof(char));
    lines[lines_required] = empty_string;
    lines_required++;
  }

  while ((0 < max_line_length) && remaining_content &&
         strlen(remaining_content)) {
    char *line = calloc(1, max_line_length + 1);

    if (strlen(remaining_content) < max_line_length) {
      remaining_content += strlen(remaining_content);
    } else {
      remaining_content += max_line_length;

      while ((start_of_line < remaining_content) &&
             !isspace(*remaining_content)) {
        --remaining_content;
      }

      if (remaining_content == start_of_line) {
        remaining_content += max_line_length;
      }
    }

    char *newline = strstr(start_of_line, "\n");

    if (NULL != newline) {
      remaining_content = newline;
    }

    strncat(line, start_of_line, remaining_content - start_of_line);

    lines[lines_required] = line;
    lines_required++;

    while ((0 < strlen(remaining_content)) && isspace(*remaining_content)) {
      remaining_content++;
    }
    start_of_line = remaining_content;
  }

  if (nout != NULL) {
    *nout = lines_required;
  }

  return lines;
}

void wrap_lines_free(char **wrapped_lines, unsigned int nlines) {
  if (wrapped_lines == NULL) {
    return;
  }

  for (int i = 0; i < nlines; i++) {
    free(wrapped_lines[i]);
  }

  free(wrapped_lines);
}
