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

void conch_listview_render(listview *lv, WINDOW *window, winrect *rect) {
  if (conch_listview_has_unread_blasts(lv)) {
    conch_status_set("unread blasts");
  }

  // Draw "thread" at left of available rect. We'll write over parts of this to
  // indicate new blasts/more blasts/selected blast.
  mvwvline(window, rect->top, rect->left,
           ACS_VLINE | COLOR_PAIR(TIMELINE_COLOR), rect->height);

  // If we're stuck to top, show a green box in the upper indicator position.
  if (lv->stick_to_top) {
    mvwvline(window, rect->top, rect->left, ACS_VLINE | COLOR_PAIR(STUCK_COLOR),
             TIMELINE_INDICATOR_HEIGHT);
  }

  // If we don't have any blasts yet, we return early.
  if (lv->blasts == NULL || lv->blasts->current == NULL) {
    return;
  }

  blast *blast = lv->top;

  // Keep track that we are scrolling from the bottom
  bool at_bottom = (lv->bottom == lv->blasts->current);

  // Indicate that prior blasts are available
  if (blast->prev) {
    mvwvline(window, rect->top, rect->left, ACS_VLINE | COLOR_PAIR(NEW_COLOR),
             TIMELINE_INDICATOR_HEIGHT);
  }

  // Loop until we run out of screen space or blasts
  int blast_width = rect->width - BLAST_THREAD_WIDTH - BLAST_PADDING_LEFT;
  int blast_x = rect->left + BLAST_THREAD_WIDTH + BLAST_PADDING_LEFT;
  int blast_y = rect->top + TIMELINE_INDICATOR_HEIGHT;
  int available_y = rect->height - 2 * TIMELINE_INDICATOR_HEIGHT;

  drawlist *dl;

  while (1) {
    int blast_lines;
    dl = conch_blast_prepare(blast, blast_width, &blast_lines);
    blast_lines = conch_blast_render(window, dl, blast_y, blast_x);

    // Draw highlight
    if (blast == lv->blasts->current) {
      mvwvline(window, blast_y, rect->left, ' ' | COLOR_PAIR(SELECTED_COLOR),
               blast_lines);
    }
    conch_drawlist_free(dl);

    blast_y += BLAST_PADDING_BOTTOM + blast_lines;
    available_y -= BLAST_PADDING_BOTTOM + blast_lines;

    // See if we hit the end of the screen and exit if we did
    if (available_y <= 0) {
      // If we did, discount the padding so that you can still select the last
      // element on screen
      available_y += BLAST_PADDING_BOTTOM;
      break;
    }

    // Exit when we run out of blasts
    if (blast->next) {
      blast = blast->next;
    } else {
      break;
    }
  }

  // Keep track of the last complete blast on screen so that the next blast key
  // press can increment top correctly. Move bottom back up the last fully
  // rendered blast if we overflowed the space.
  lv->bottom = blast;
  if (available_y < 0) {
    lv->bottom = lv->bottom->prev;
  }

  // If we started at the bottom we should remain at the last drawn item.
  if (at_bottom && lv->bottom) {
    lv->blasts->current = lv->bottom;
  }

  // Indicate that more blasts are available
  if (blast->next) {
    mvwvline(window, rect->bottom, rect->left,
             ACS_VLINE | COLOR_PAIR(NEW_COLOR), TIMELINE_INDICATOR_HEIGHT);
  }
}
