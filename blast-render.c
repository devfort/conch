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

char **conch_blast_prepare(blast *blast, int width, int *nlines) {
  *nlines = 0;

  if (blast == NULL) {
    return NULL;
  }

  char **wrapped_blast = wrap_lines(blast->content, width);

  // Find NULL at end of lines
  while (wrapped_blast[*nlines]) {
    (*nlines)++;
  }

  if (blast->attachment != NULL) {
    // TODO: wrap this
    wrapped_blast[(*nlines)++] = strclone(blast->attachment);
  }

  char const *const extended_marker =
      blast->extended ? BLAST_EXTENDED_MARKER : "";
  char *attribution = malloc(1024 * sizeof(char));
  // TODO: wrap/truncate this
  snprintf(attribution, 1024, "—%s at %s %s", blast->user, blast->posted_at,
           extended_marker);
  wrapped_blast[(*nlines)++] = attribution;

  // Terminate the blast lines
  wrapped_blast[*nlines] = NULL;

  return wrapped_blast;
}
