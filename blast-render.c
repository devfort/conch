#include <curses.h>
#include <stdlib.h>
#include <string.h>

#include "blastlist.h"
#include "colors.h"
#include "blast-render.h"
#include "strutils.h"

int conch_blast_render(WINDOW *window, drawlist *l, int y, int x) {
  int i;
  for (i = 0; l->content[i] != NULL; i++) {
    mvwaddnstr(window, y + i, x, l->content[i], strlen(l->content[i]));
  }
  return i;
}

drawlist *conch_blast_prepare(blast *blast, int width, int *nlines) {
  *nlines = 0;

  if (blast == NULL) {
    return NULL;
  }

  drawlist *instructions = calloc(1, sizeof(drawlist));
  char *message;
  if (blast->extended) {
    message = strcopycat(blast->content, BLAST_EXTENDED_MARKER);
  } else {
    message = blast->content;
  }

  char **wrapped_blast = wrap_lines(message, width);

  if (blast->extended) {
    free(message);
  }

  // Find NULL at end of lines
  while (wrapped_blast[*nlines]) {
    (*nlines)++;
  }

  if (blast->attachment != NULL) {
    // TODO: wrap this
    wrapped_blast[(*nlines)++] = strclone(blast->attachment);
  }

  char *attribution = malloc(1024 * sizeof(char));
  // TODO: wrap/truncate this
  snprintf(attribution, 1024, "—%s at %s", blast->user, blast->posted_at);
  wrapped_blast[(*nlines)++] = attribution;

  // Terminate the blast lines
  wrapped_blast[*nlines] = NULL;

  instructions->content = wrapped_blast;

  return instructions;
}

void conch_drawlist_free(drawlist *l) {
  if (l != NULL) {
    wrap_lines_free(l->content);
  }
  free(l);
}
