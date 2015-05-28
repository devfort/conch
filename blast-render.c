#include <curses.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "blastlist.h"
#include "colors.h"
#include "blast-render.h"
#include "strutils.h"

/*
 * Draw as many lines from blast_lines as possible on window, starting at
 * coordinates (y, x) and continuing for at most maxlines.
 *
 * If invert is true, conch_blast_render will render from the bottom of the
 * blast up.
 *
 * Returns the number of lines drawn.
 */
unsigned int conch_blast_render(WINDOW *window, drawlist *l,
                                unsigned int nlines, int y, int x,
                                unsigned int maxlines, bool invert) {
  unsigned int rendered_lines = 0;
  if (invert) {
    for (int i = nlines - 1; i >= 0 && rendered_lines < maxlines; i--) {
      rendered_lines++;
      mvwaddnstr(window, y--, x, l->content[i], strlen(l->content[i]));
    }
  } else {
    for (int i = 0; i < nlines && rendered_lines < maxlines; i++) {
      rendered_lines++;
      mvwaddnstr(window, y++, x, l->content[i], strlen(l->content[i]));
    }
  }
  return rendered_lines;
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
