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

int NORMAL_COLOR;
int NEW_COLOR;
int SELECTED_COLOR;
int TIMELINE_COLOR;
int STUCK_COLOR;

void conch_init_colors() { start_color(); }
