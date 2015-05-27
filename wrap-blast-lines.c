#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#include "wrap-blast-lines.h"

char **conch_blast_lines(char *blast, int available_width) {
  char **lines = malloc(1024);
  int lines_required = 0;
  char *start_of_line, *remaining_content;
  start_of_line = remaining_content = blast;

  if (NULL == blast || 0 == strlen(blast)) {
    char *empty_string = calloc(1, 1);
    lines[lines_required] = empty_string;
    lines_required++;
  }

  while (remaining_content && strlen(remaining_content)) {
    char *line = calloc(1, available_width);

    if (available_width < strlen(remaining_content)) {
      remaining_content += available_width;

      while (!isspace(*remaining_content)) {
        --remaining_content;
      }
    } else {
      remaining_content += strlen(remaining_content);
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

void conch_blast_lines_free(char **blast_lines) {
  if (blast_lines == NULL) {
    return;
  }

  for (int i = 0; blast_lines[i]; i++) {
    free(blast_lines[i]);
  }

  free(blast_lines);
}
