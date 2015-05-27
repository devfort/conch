#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#include "wrap-blast-lines.h"

void conch_blast_lines(char *blast_lines[], char *blast, int available_width) {

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

    blast_lines[lines_required] = line;
    lines_required++;

    while (isspace(*remaining_content)) {
      remaining_content++;
    }
    start_of_line = remaining_content;
  }
  if (strlen(remaining_content) < available_width) {
    blast_lines[lines_required] = remaining_content;
    lines_required++;
  }
  blast_lines[lines_required] = NULL;
}
