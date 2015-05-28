#include <curses.h>
#include <stdlib.h>
#include <string.h>

#include "blastlist.h"
#include "blast-render.h"
#include "strutils.h"

#define BLAST_MARGIN_LEFT 1

void conch_blast_render(WINDOW *window, char **blast_lines, int y, int gutter_x,
                        chtype highlight) {
  // Gutter is 1 character wide because we use mvwvline
  const int gutter_width = 1;
  const int blast_x = gutter_x + gutter_width + BLAST_MARGIN_LEFT;
  int i;
  for (i = 0; blast_lines[i]; i++) {
    mvwaddnstr(window, y + i, blast_x, blast_lines[i], strlen(blast_lines[i]));
  }
  int number_of_blast_lines = i;
  mvwvline(window, y, gutter_x, highlight, number_of_blast_lines);
}

char **conch_generate_wrapped_blast(blast *blast, int max_line_length) {
  char **wrapped_blast = wrap_lines(blast->content, max_line_length);

  int line;
  for (line = 0; wrapped_blast[line]; line++)
    ;

  if (blast->attachment != NULL) {
    char *attachment = malloc(strlen(blast->attachment) + 1);
    strcpy(attachment, blast->attachment);
    wrapped_blast[line] = attachment;
    line++;
  }

  char *attribution_string = malloc(1024 * sizeof(char));
  snprintf(attribution_string, 1024, "—%s at %s", blast->user,
           blast->posted_at);
  wrapped_blast[line] = attribution_string;

  if (blast->extended) {
    // Show a marker to indicate the blast has a code block with it
    // TODO: Find a better/clearer indicator
    wrapped_blast[line] = strcat(attribution_string, "…");
  }
  line++;

  wrapped_blast[line] = NULL;

  return wrapped_blast;
}
