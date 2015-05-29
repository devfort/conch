#include <curses.h>
#include <errno.h>
#include <string.h>

#include "explode.h"
#include "help.h"
#include "strutils.h"

#define HELP_DIALOG_VSIZE 0.66
#define HELP_DIALOG_HSIZE 0.66
#define HELP_DIALOG_VBORDER 1
#define HELP_DIALOG_HBORDER 1
#define HELP_DIALOG_VPADDING 1
#define HELP_DIALOG_HPADDING 2

// AWOOGA AWOOGA FIXME: I've indented the following with unicode non-breaking
// spaces to work around the fact that wrap_lines currently strips leading
// spaces.

// clang-format off
static char const *help_text = {
  "Global keyboard shortcuts:\n"
  "\n"
  "  q         quit\n"
  "  @         toggle screensaver\n"
  "\n\n"
  "Timeline shortcuts:\n"
  "\n"
  "  j         select next blast\n"
  "  k         select previous blast\n"
  "  ENTER     view blast detail\n"
  "  i         enter insert mode\n"
  "  0 g       jump to top\n"
  "  G         jump to bottom\n"
  "  . TAB     jump to next unread\n"
  "  SPC PGDN  next page\n"
  "  PGUP      prev page\n"
  "  s         follow latest blasts\n"
  "  /         search forward\n"
  "  n         repeat last search\n"
  "\n\n"
  "Detail view shortcuts:\n"
  "\n"
  "  h j k l   scroll text\n"
  "  e v       open text in $EDITOR\n"
  "  ENTER     return to timeline\n"
};
// clang-format on

void conch_help_render(WINDOW *w) {
  WINDOW *dia, *pad;
  unsigned int winh, winw; /* global window height, width */
  unsigned int diah, diaw; /* dialog on-screen height, width */
  unsigned int diay, diax; /* dialog on-screen origin */
  unsigned int padh, padw; /* pad on-screen height, width */
  unsigned int pady, padx; /* pad on-screen origin */
  unsigned int padnl;      /* pad number of lines */
  char **text;

  winh = getmaxy(w);
  winw = getmaxx(w);

  diah = (unsigned int)(winh * HELP_DIALOG_VSIZE);
  diaw = (unsigned int)(winw * HELP_DIALOG_HSIZE);
  diay = winh / 2 - diah / 2;
  diax = winw / 2 - diaw / 2;

  padh = diah - 2 * (HELP_DIALOG_VBORDER + HELP_DIALOG_VPADDING);
  padw = diaw - 2 * (HELP_DIALOG_HBORDER + HELP_DIALOG_HPADDING);
  pady = diay + HELP_DIALOG_VBORDER + HELP_DIALOG_VPADDING;
  padx = diax + HELP_DIALOG_HBORDER + HELP_DIALOG_HPADDING;

  text = wrap_lines(help_text, padw, &padnl);

  dia = newpad(diah, diaw);
  if (!dia) {
    fatal_error("conch_help_render: Could not create help dialog (%s)",
                strerror(errno));
  }
  pad = newpad(padnl, padw);
  if (!pad) {
    fatal_error("conch_help_render: Could not create help textpad (%s)",
                strerror(errno));
  }

  box(dia, 0, 0);
  mvwaddstr(dia, 0, 2, " conch help ");
  pnoutrefresh(dia, 0, 0, diay, diax, diay + diah, diax + diaw);

  for (unsigned int i = 0; i < padnl; i++) {
    mvwaddstr(pad, i, 0, text[i]);
  }
  pnoutrefresh(pad, 0, 0, pady, padx, pady + padh, padx + padw);

  delwin(pad);
  delwin(dia);
}
