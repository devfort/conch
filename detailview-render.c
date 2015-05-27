#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "detailview-render.h"
#include "listview.h"
#include "listview-render.h"
#include "wordwrap.h"
#include "strutils.h"

extern window_chrome_s chrome;

extern char **generate_blast_lines(WINDOW *window, int available_width, int y,
                                   int gutter_x, blast *blast,
                                   chtype highlight);

int calculate_summary_blast_height(blast *blast, int usable_window_width) {
  wordwrap_s wrap;
  init_wordwrap(&wrap, blast->content, usable_window_width);

  int lines = 0;
  for (token_s *token = wordwrap(&wrap); token != NULL;
       token = wordwrap(&wrap)) {
    lines = token->y;
  }

  if (blast->attachment != NULL) {
    lines++;
  }

  // We want the blast, plus one line for author information. 1 because line
  // is
  // zero indexed, and +1 again to make room for the author
  return (lines + 1) + 1;
}

void fatal_error(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  endwin();
  vfprintf(stderr, fmt, args);
  fprintf(stderr, "\n");
  va_end(args);
  abort();
}

void conch_detailview_render(detailview *v, WINDOW *window, winrect *rect) {

  // Number of chars used to display line numbers
  const int line_no_width = 4;

  // TODO: rename this method call vv
  int summary_height = calculate_summary_blast_height(v->blastlist->current,
                                                      rect->right - rect->left);

  // Right coordinate is inclusive. Thanks ncurses. thurses.
  int available_width = rect->width, available_height = rect->height;

  int code_height = 0, code_width = 0;
  if (v->blastlist->current->extended) {
    code_height = count_lines_and_find_length_of_longest(
        v->blastlist->current->extended, &code_width);
    // Add an extra line in between the blast and the code block
    summary_height++;

    // Clamp the code_column_offset here. We might have resized since we last
    // pressed a key, so this is the only place we can be sure to clamp
    // correctly
    if (v->code_column_offset + (available_width - line_no_width) >
        code_width) {
      v->code_column_offset = code_width - (available_width - line_no_width);
    }
    if (v->code_column_offset < 0) {
      v->code_column_offset = 0;
    }
  }

  if (v->line_offset + available_height > summary_height + code_height) {
    v->line_offset = summary_height + code_height - available_height;
  }
  if (v->line_offset < 0) {
    v->line_offset = 0;
  }

  WINDOW *pad = newpad(summary_height + code_height, available_width);
  if (!pad) {
    fatal_error("conch_detailview_render: Could not create vertical pad");
  }

  // Render the blast at 0,0 on the pad
  char **blast_lines = generate_blast_lines(pad, available_width, 0, 0, v->blastlist->current, ' ');
  wrap_lines_free(blast_lines);

  // Render the chrome to ncurses internal state, but don't render to the
  // screen.
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
  if (pnoutrefresh(pad, v->line_offset, 0, rect->top, rect->left, rect->bottom,
                   rect->right) == ERR) {
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
    if (waddstr(code_pad, v->blastlist->current->extended) == ERR) {
      fatal_error("conch_detailview_render: waddstr to code_pad failed! Pad "
                  "size %d,%d",
                  code_height + 1, code_width);
    }

    // Where the top of the code pad should be drawn to - if we have scrolled
    // up this could be < 0
    int code_pad_top = rect->top + summary_height - v->line_offset,
        code_scroll_top = 0;

    // rect is unsigned, so we need to force it to signed so the comparison
    // makes sense
    if (code_pad_top < (int)rect->top) {
      code_scroll_top = rect->top - code_pad_top;
      code_pad_top = rect->top;
    }

    int left = rect->left + line_no_width;
    if (pnoutrefresh(code_pad, code_scroll_top, v->code_column_offset,
                     code_pad_top, left, rect->bottom, rect->right) == ERR) {
      fatal_error("Could not output codepad");
    }
    delwin(code_pad);
  }

  delwin(pad);
}
