#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "anigif-render.h"
#include "blast-render.h"
#include "detailview-render.h"
#include "listview.h"
#include "listview-render.h"
#include "strutils.h"
#include "explode.h"

#define PADDING_TOP 1
#define PADDING_BOTTOM 1

void conch_detailview_render(detailview *v, WINDOW *window, winrect *rect) {
  // Where to put the pad on screen
  const int pad_screen_top = rect->top + PADDING_TOP;
  const int pad_screen_bottom = rect->bottom - PADDING_BOTTOM;
  const int pad_screen_height = rect->height - PADDING_TOP - PADDING_BOTTOM;

  // Number of chars used to display line numbers
  const int line_no_width = 4;
  const bool dont_show_extended_markers = false;

  drawlist *dl =
      conch_blast_prepare(v->blastlist->current, rect->width - line_no_width,
                          dont_show_extended_markers);

  int summary_height = dl->nlines;

  int code_height = 0, code_width = 0;
  if (v->blastlist->current->extended) {
    code_height = count_lines_and_find_length_of_longest(
        v->blastlist->current->extended, &code_width);
    // Add an extra line in between the blast and the code block
    summary_height++;

    // Clamp the code_column_offset here. We might have resized since we last
    // pressed a key, so this is the only place we can be sure to clamp
    // correctly
    if (v->code_column_offset + (rect->width - line_no_width) > code_width) {
      v->code_column_offset = code_width - (rect->width - line_no_width);
    }
    if (v->code_column_offset < 0) {
      v->code_column_offset = 0;
    }
  }

  if (v->line_offset + pad_screen_height > summary_height + code_height) {
    v->line_offset = summary_height + code_height - pad_screen_height;
  }
  if (v->line_offset < 0) {
    v->line_offset = 0;
  }

  WINDOW *pad = newpad(summary_height + code_height, rect->width);
  if (!pad) {
    fatal_error("conch_detailview_render: Could not create vertical pad");
  }

  // Render the blast at 0,0 on the pad
  conch_blast_render(pad, dl, dl->nlines, 0, line_no_width, dl->nlines,
                     false);
  conch_drawlist_free(dl);

  // Flush renders to ncurses internal state.
  wnoutrefresh(window);

  if (v->blastlist->current->extended) {
    for (unsigned int line = 1; line <= code_height; line++) {
      mvwprintw(pad, line + summary_height - 1, 0, "%03d", line);
    }
  }
  // Render the blast and also update the terminal with the combined state
  //
  // This pnoutrefresh splits the `if blast->extended` because we need to
  // output the linenumbers (which pnoutrefreshdoes) before we render the code
  // lines which need to overlay.
  if (pnoutrefresh(pad, v->line_offset, 0, pad_screen_top, rect->left,
                   pad_screen_bottom, rect->right) == ERR) {
    fatal_error("conch_detailview_render: Could not output pad to screen");
  }

  if (v->blastlist->current->extended) {
    // waddstr will fail if we try to print outside of the pad. But it counts
    // \n as a character on the previous line.
    //
    // TODO: Count the final \n in count_lines_and_find_length_of_longest then
    // we can remove +1 from code_height
    WINDOW *code_pad = newpad(code_height + 1, code_width + 1);
    if (!code_pad) {
      fatal_error(
          "conch_detailview_render: Could not create code pad h: %d, w: %d",
          code_height, code_width);
    }

    // There might be '\r' in the code snippet. Annoyingly when ncurses prints
    // a '\r' it erases the line it just printed. So as a bit of a hack replace
    // every '\r' with a ' '
    char *string, *token;
    string = token = strclone(v->blastlist->current->extended);
    while ((token = strchr(token, '\r')) != NULL) {
      *token = ' ';
    }

    if (waddstr(code_pad, string) == ERR) {
      fatal_error("conch_detailview_render: waddstr to code_pad failed! Pad "
                  "size %d,%d",
                  code_height + 1, code_width);
    }
    free(string);

    // Where the top of the code pad should be drawn to - if we have scrolled
    // up this could be < 0
    int code_pad_top = pad_screen_top + summary_height - v->line_offset,
        code_scroll_top = 0;

    // rect is unsigned, so we need to force it to signed so the comparison
    // makes sense
    if (code_pad_top < pad_screen_top) {
      code_scroll_top = pad_screen_top - code_pad_top;
      code_pad_top = pad_screen_top;
    }

    int left = rect->left + line_no_width;
    if (pnoutrefresh(code_pad, code_scroll_top, v->code_column_offset,
                     code_pad_top, left, pad_screen_bottom,
                     rect->right) == ERR) {
      fatal_error("Could not output codepad");
    }
    delwin(code_pad);
  }

  delwin(pad);

  // TODO: Render me properly into the pad so we can cope with an image and a
  // code blast!
  if (v->anigif) {
    // Leave some padding between the image and the blast
    summary_height++;

    rect->height -= summary_height;
    rect->top += summary_height;
    anigif_render_frame(v->anigif, window, rect);
  }
}
