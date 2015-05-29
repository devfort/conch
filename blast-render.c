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
        render_extended_marker(window, y, x + l->marker_rel_x);
      }
      y--;
    }

  } else {
    for (int i = 0; i < nlines && rendered_lines < maxlines; i++) {
      rendered_lines++;
      mvwaddnstr(window, y, x, l->content[i], strlen(l->content[i]));
      if (l->has_marker && l->content_last_line == i) {
        render_extended_marker(window, y, x + l->marker_rel_x);
      }
      y++;
    }
  }
  return rendered_lines;
}

drawlist *conch_blast_prepare(blast *blast, int width, int *nlines,
                              bool display_marker) {
  *nlines = 0;

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

  char **wrapped_blast = wrap_lines(message, width);

  // Find NULL at end of lines
  while (wrapped_blast[*nlines]) {
    (*nlines)++;
  }

  // We've now wrapped the lines, find the marker on the last line.
  if (display_marker && blast->extended) {
    free(message);
    instructions->has_marker = true;

    int last_line_idx = *nlines - 1;
    char *last_line = wrapped_blast[last_line_idx];
    int len = strlen(last_line);

    instructions->content_last_line = last_line_idx;
    instructions->marker_rel_x = len - strlen(BLAST_EXTENDED_MARKER);

    // And lets be tidy and not draw it in white, then re-draw in color
    last_line[instructions->marker_rel_x] = 0;
  }

  if (blast->attachment != NULL) {
    wrapped_blast[(*nlines)++] = strcopytrunc(blast->attachment, width);
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
