#include <curses.h>
#include <stdlib.h>
#include <string.h>

#include "blastlist.h"
#include "colors.h"
#include "listview.h"
#include "listview-render.h"
#include "strutils.h"

#include "blast-render.h"
#include "render.h"

#define BLAST_THREAD_WIDTH 1 /* Drawn by mvwvline as single char wide line */
#define BLAST_PADDING_LEFT 1
#define BLAST_PADDING_BOTTOM 2

static void conch_highlight_render(WINDOW *w, unsigned int y, unsigned int x,
                                   unsigned int height, bool invert) {
  if (invert) {
    y = y - height + 1; // +1 so we always draw on line y (as passed)
  }
  mvwvline(w, y, x, ' ' | COLOR_PAIR(SELECTED_COLOR), height);
}

/*
 * Working out exactly when we can draw a blast (either in its entirety or
 * partially) is a bit tricky, with plenty of potential for off-by-one errors.
 *
 * The following are configurable:
 *
 * - the number of lines of chrome above the passed rect (i.e. rect->top)
 * - the width of the indicators above and below the blast render area (i.e.
 *   TIMELINE_INDICATOR_HEIGHT
 *
 * In the example below, there are two lines of chrome and two lines for the
 * indicators.
 *
 *     0 ----- CHROME -----
 *     1 ----- CHROME -----
 *     2 ---- INDICATOR ---
 *     3 ---- INDICATOR ---
 *     4                     <- can draw 2 lines of blasts from here
 *     5                     <- can draw 1 line of blasts from here
 *     6 ---- INDICATOR ---
 *     7 ---- INDICATOR ---
 *     8 ----- CHROME -----
 *     9 ----- CHROME -----
 *
 * The available area for drawing blasts comprises lines 4 and 5. The passed
 * rect has the following (relevant) properties:
 *
 *     rect->top = 2
 *     rect->bottom = 7
 *     rect->height = 6
 *
 * From these numbers and the configurable parameters, we want to know two
 * things:
 *
 * 1. is it time to break out of the loop?
 * 2. what is the maximum number of lines to draw?
 *
 * Defining the blast area top and bottom as:
 *
 *     blast_area_top = rect->top + TIMELINE_INDICATOR_HEIGHT
 *     blast_area_bottom = rect->bottom - TIMELINE_INDICATOR_HEIGHT
 *
 * We can then calculate these two things as follows. When drawing from the
 * top:
 *
 * 1. cursor_y > blast_area_bottom
 * 2. blast_area_bottom - cursor_y + 1
 *
 * When drawing from the bottom:
 *
 * 1. cursor_y < blast_area_top
 * 2. cursor_y - blast_area_top + 1
 */
void conch_listview_render(listview *lv, WINDOW *window, winrect *rect) {
  if (conch_listview_has_unread_blasts(lv)) {
    conch_status_set("unread blasts");
  }

  // Draw "thread" at left of available rect. We'll write over parts of this to
  // indicate new blasts/more blasts/selected blast.
  mvwvline(window, rect->top, rect->left,
           ACS_VLINE | COLOR_PAIR(TIMELINE_COLOR), rect->height);

  // If we're stuck to top, show a green box in the upper indicator position.
  if (lv->stick_to_head) {
    mvwvline(window, rect->top, rect->left, ACS_VLINE | COLOR_PAIR(STUCK_COLOR),
             TIMELINE_INDICATOR_HEIGHT);
  }

  // If we don't have any blasts yet, we return early.
  if (lv->blasts == NULL || lv->blasts->current == NULL) {
    return;
  }

  blast *blast = lv->top;

  // If the current blast is at the bottom of the rendered area, we draw from
  // the bottom up, otherwise, we draw from the top down.
  if (lv->render_from_bottom) {
    blast = lv->bottom;
  }

  int blast_area_top = rect->top + TIMELINE_INDICATOR_HEIGHT;
  int blast_area_bottom = rect->bottom - TIMELINE_INDICATOR_HEIGHT;
  int blast_width = rect->width - BLAST_THREAD_WIDTH - BLAST_PADDING_LEFT;

  int cursor_y = blast_area_top;
  int cursor_x = rect->left + BLAST_THREAD_WIDTH + BLAST_PADDING_LEFT;
  if (lv->render_from_bottom) {
    cursor_y = blast_area_bottom;
  }

  int blast_lines;
  int rendered_blast_lines;
  const bool display_extended_markers = true;

  while (1) {
    // maxlines is the maximum number of lines of blast that we are now allowed
    // to render. See the long comment above for details of this calculation.
    int maxlines = blast_area_bottom - cursor_y + 1;
    if (lv->render_from_bottom) {
      maxlines = cursor_y - blast_area_top + 1;
    }

    drawlist *dl =
        conch_blast_prepare(blast, blast_width, display_extended_markers);
    blast_lines = dl->nlines;
    rendered_blast_lines =
        conch_blast_render(window, dl, dl->nlines, cursor_y, cursor_x, maxlines,
                           lv->render_from_bottom);
    conch_drawlist_free(dl);

    // Draw highlight
    if (blast == lv->blasts->current) {
      conch_highlight_render(window, cursor_y, rect->left, rendered_blast_lines,
                             lv->render_from_bottom);
    }

    if (lv->render_from_bottom) {
      cursor_y -= BLAST_PADDING_BOTTOM + rendered_blast_lines;
    } else {
      cursor_y += BLAST_PADDING_BOTTOM + rendered_blast_lines;
    }

    if (lv->render_from_bottom && cursor_y < blast_area_top) {
      break;
    } else if (cursor_y > blast_area_bottom) {
      break;
    }

    // Exit when we run out of blasts
    if (lv->render_from_bottom && blast->prev) {
      blast = blast->prev;
    } else if (blast->next) {
      blast = blast->next;
    } else {
      break;
    }
  }

  // Allow the view key handlers to find out whether or not the last blast
  // rendered (at either the top or bottom of the screen, depending on render
  // direction) overflowed the available blast render area.
  lv->render_overflow = (blast_lines != rendered_blast_lines);

  // Set lv->top/lv->bottom to the first/last blast on screen, even if only
  // partially rendered.
  if (lv->render_from_bottom) {
    lv->top = blast;
  } else {
    lv->bottom = blast;
  }

  // Indicate that more blasts are available above or below the rendered area:
  if (lv->top->prev) {
    mvwvline(window, rect->top, rect->left, ACS_VLINE | COLOR_PAIR(NEW_COLOR),
             TIMELINE_INDICATOR_HEIGHT);
  }
  if (lv->bottom->next) {
    mvwvline(window, rect->bottom, rect->left,
             ACS_VLINE | COLOR_PAIR(NEW_COLOR), TIMELINE_INDICATOR_HEIGHT);
  }
}
