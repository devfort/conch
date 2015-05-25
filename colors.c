#include <curses.h>

#include "colors.h"

int curses_colors[] = {
  /* Standard curses colours */
  COLOR_BLACK,     COLOR_BLUE,        COLOR_GREEN,      COLOR_CYAN,
  COLOR_RED,       COLOR_MAGENTA,     COLOR_YELLOW,     COLOR_WHITE,
  /* Extra values for xterm-16color */
  COLOR_BLACK + 8, COLOR_BLUE + 8,    COLOR_GREEN + 8,  COLOR_CYAN + 8,
  COLOR_RED + 8,   COLOR_MAGENTA + 8, COLOR_YELLOW + 8, COLOR_WHITE + 8
};

void conch_init_colors() {
  start_color();
  use_default_colors();

  init_color(100, 333, 333, 333);

  init_pair(NORMAL_COLOR, COLOR_WHITE, COLOR_BLACK);
  init_pair(NEW_COLOR, COLOR_BLUE, COLOR_BLUE);
  init_pair(SELECTED_COLOR, COLOR_WHITE, COLOR_RED);
  init_pair(TIMELINE_COLOR, 100, -1);
  init_pair(STUCK_COLOR, COLOR_GREEN, COLOR_GREEN);
}
