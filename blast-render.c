#include <curses.h>
#include <stdlib.h>
#include <string.h>

#include "blastlist.h"
#include "blast-render.h"
#include "strutils.h"

int conch_blast_render(WINDOW *window, char **blast_lines, int y, int x) {
  int i;
  for (i = 0; blast_lines[i] != NULL; i++) {
    mvwaddnstr(window, y + i, x, blast_lines[i], strlen(blast_lines[i]));
  }
  return i;
}

char **conch_generate_wrapped_blast(blast *blast, int max_line_length) {
  int line = 0;
  char **wrapped_blast = wrap_lines(blast->content, max_line_length);

  // Find NULL at end of lines
  while (wrapped_blast[line]) {
    line++;
  }

  if (blast->attachment != NULL) {
    wrapped_blast[line++] = strclone(blast->attachment);
  }

  char const *const attachment_marker = blast->extended ? "[@]" : "   ";
  char *attribution = malloc(1024 * sizeof(char));
  snprintf(attribution, 1024, "—%s at %s %s", blast->user, blast->posted_at,
           attachment_marker);
  wrapped_blast[line++] = attribution;

  // Terminate the blast lines
  wrapped_blast[line++] = NULL;

  return wrapped_blast;
}
