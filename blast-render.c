#include <curses.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "blastlist.h"
#include "colors.h"
#include "blast-render.h"
#include "strutils.h"

#define BLAST_EXTENDED_MARKER "[...]"
#define BLAST_EXTENDED_MARKER_WITH_SPACE (" " BLAST_EXTENDED_MARKER)

static void render_extended_marker(WINDOW *window, unsigned int y,
                                   unsigned int x) {
  wattron(window, COLOR_PAIR(EXTENDED_COLOR) | A_UNDERLINE);
  mvwaddstr(window, y, x, BLAST_EXTENDED_MARKER);
  wattroff(window, COLOR_PAIR(EXTENDED_COLOR) | A_UNDERLINE);
}

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
      mvwaddnstr(window, y, x, l->content[i], strlen(l->content[i]));

      if (l->has_marker && l->content_last_line == i) {
        render_extended_marker(window, y, x + l->last_line_length);
      }
      y--;
    }

  } else {
    for (int i = 0; i < nlines && rendered_lines < maxlines; i++) {
      rendered_lines++;
      mvwaddnstr(window, y, x, l->content[i], strlen(l->content[i]));
      if (l->has_marker && l->content_last_line == i) {
        render_extended_marker(window, y, x + l->last_line_length);
      }
      y++;
    }
  }
  return rendered_lines;
}

drawlist *conch_blast_prepare(blast *blast, int width, bool display_marker) {
  if (blast == NULL) {
    return NULL;
  }

  drawlist *instructions = calloc(1, sizeof(drawlist));
  char *message;

  // Even though the "[...]" marker is going to be printed in a different
  // colour, passing it in to wrap_lines is the easiest way of working out
  // where it needs to be printed.
  if (display_marker && blast->extended) {
    message = strcopycat(blast->content, BLAST_EXTENDED_MARKER_WITH_SPACE);
  } else {
    message = blast->content;
  }

  unsigned int nlines;
  char **wrapped_blast = wrap_lines(message, width, &nlines);

  // Ensure we have space for another pointer at the end of the list of lines to
  // account for the attribution.
  wrapped_blast = realloc(wrapped_blast, (nlines + 1) * sizeof(char *));

  // We've now wrapped the lines, find the marker on the last line.
  if (display_marker && blast->extended) {
    free(message);
    instructions->has_marker = true;

    int last_line_idx = nlines - 1;
    char *last_line = wrapped_blast[last_line_idx];

    instructions->content_last_line = last_line_idx;
    instructions->last_line_length =
        strlen(last_line) - strlen(BLAST_EXTENDED_MARKER);

    // We do not have a unicode safe strlen function,
    // so if there is unicode in the last line of our string
    // the render function will draw it in the wrong place.
    // Until we have unicode support, therefore, we need to
    // remove the marker at this point otherwise they may get
    // drawn in different places.
    last_line[instructions->last_line_length] = 0;
  }

  if (blast->attachment != NULL) {
    wrapped_blast[nlines++] = strcopytrunc(blast->attachment, width);
    // We just stole the attribution's slot, so add another line.
    wrapped_blast = realloc(wrapped_blast, (nlines + 1) * sizeof(char *));
  }

  char *attribution = malloc(1024 * sizeof(char));
  snprintf(attribution, 1024, "—%s at %s", blast->user, blast->posted_at);
  wrapped_blast[nlines++] = strcopytrunc(attribution, width);
  free(attribution);

  instructions->nlines = nlines;
  instructions->content = wrapped_blast;

  return instructions;
}

void conch_drawlist_free(drawlist *l) {
  if (l == NULL) {
    return;
  }
  for (int i = 0; i < l->nlines; i++) {
    free(l->content[i]);
  }
  free(l);
}
