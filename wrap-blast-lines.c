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

  while (strlen(remaining_content) > available_width) {
    char *line = calloc(1, available_width);
    remaining_content += available_width;

    while (!isspace(*remaining_content)) {
      --remaining_content;
    }

    strncat(line, start_of_line, remaining_content - start_of_line);

    lines[lines_required] = line;
    lines_required++;

    while (isspace(*remaining_content)) {
      remaining_content++;
    }
    start_of_line = remaining_content;
  }
  if (strlen(remaining_content) < available_width) {
    lines[lines_required] = remaining_content;
    lines_required++;
  }
  lines[lines_required] = NULL;

  return lines;
}
