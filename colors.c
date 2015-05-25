#include <curses.h>

#include "colors.h"

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
