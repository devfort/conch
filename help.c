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
#define HELP_DIALOG_HPADDING 1
#define NBSP "\xa0"

static char const *help_text = {
  "Global keyboard shortcuts:\n"
  "\n" NBSP NBSP "q         quit\n" NBSP NBSP "@         toggle screensaver\n"
  "\n\n"
  "Timeline shortcuts:\n"
  "\n" NBSP NBSP "j         select next blast\n" NBSP NBSP
  "k         select previous blast\n" NBSP NBSP
  "ENTER     view blast detail\n" NBSP NBSP
  "i         enter insert mode\n" NBSP NBSP "0 g       jump to top\n" NBSP NBSP
  "G         jump to bottom\n" NBSP NBSP
  ". TAB     jump to next unread\n" NBSP NBSP "SPC PGDN  next page\n" NBSP NBSP
  "PGUP      prev page\n" NBSP NBSP "s         follow latest blasts\n" NBSP NBSP
  "/         search forward\n" NBSP NBSP "n         repeat last search\n"
  "\n\n"
  "Detail view shortcuts:\n"
  "\n" NBSP NBSP "h j k l   scroll text\n" NBSP NBSP
  "e v       open text in $EDITOR\n" NBSP NBSP
  "ENTER     return to timeline\n"
};

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
