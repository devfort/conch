#include <assert.h>
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

int count_lines_and_find_length_of_longest(const char *string,
                                           int *out_longest_line) {
  *out_longest_line = 0;
  int lines = 0;

  char const *start_of_line = string;

  string = strchr(string, '\n');
  while (string != NULL) {
    int this_line_len = string - start_of_line;
    if (this_line_len > *out_longest_line) {
      *out_longest_line = this_line_len;
    }
    lines++;

    // Skip over the new line
    string += 1;
    start_of_line = string;
    string = strchr(string, '\n');
  }

  // handle last line - which could have no trailing new-line ("...\nabc\0" vs
  // "...\n\0")
  if (*start_of_line != '\0') {
    int this_line_len = strlen(start_of_line);
    if (this_line_len > *out_longest_line) {
      *out_longest_line = this_line_len;
    }
    lines++;
  }

  return lines;
}
