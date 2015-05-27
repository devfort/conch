#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "strutils.h"

char *strclone(char *c) {
  if (c == NULL) {
    return NULL;
  }

  char *target = malloc(strlen(c) + 1);

  if (target == NULL) {
    fprintf(stderr, "strclone: could not alloc\n");
    abort();
  }

  strcpy(target, c);
  return target;
}

char *strcopycat(char *c, char *d) {
  assert(c != NULL);
  assert(d != NULL);

  int n = strlen(c);
  char *target = malloc(n + strlen(d) + 1);

  if (target == NULL) {
    fprintf(stderr, "strcopycat: could not alloc\n");
    abort();
  }

  strcpy(target, c);
  strcpy(target + n, d);
  return target;
}

char* stralleycat(int count, char **strs) {
  int i = 0, size = 0;
  int idx = 0;
  for(i=0; i < count; i++) {
    size += strlen(strs[i]) + 1;
  }

  char *result = malloc(size*sizeof(char));
  if (result == NULL) {
    fprintf(stderr, "stralleycat: could not alloc\n");
    abort();
  }

  for(i=0; i < count; i++) {
    strcpy(result + idx, strs[i]);
    idx += strlen(strs[i]);
    result[idx] = ' ';
    idx += 1;
  }
  result[idx-1] = 0;
  return result;
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

char **wrap_lines(char *text, int max_line_length) {
  char **lines = malloc(1024 * sizeof(char *));
  int lines_required = 0;
  char *start_of_line, *remaining_content;
  start_of_line = remaining_content = text;

  if (NULL == text || 0 == strlen(text) || max_line_length <= 0) {
    char *empty_string = calloc(1, 1);
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

    strncat(line, start_of_line, remaining_content - start_of_line);

    lines[lines_required] = line;
    lines_required++;

    while ((0 < strlen(remaining_content)) && isspace(*remaining_content)) {
      remaining_content++;
    }
    start_of_line = remaining_content;
  }
  lines[lines_required] = NULL;

  return lines;
}

void wrap_lines_free(char **wrapped_lines) {
  if (wrapped_lines == NULL) {
    return;
  }

  for (int i = 0; wrapped_lines[i]; i++) {
    free(wrapped_lines[i]);
  }

  free(wrapped_lines);
}
