#ifndef _COLORS_H
#define _COLORS_H

enum conch_color {
  NORMAL_COLOR = 0,
  NEW_COLOR = 2,
  SELECTED_COLOR = 3,
  TIMELINE_COLOR = 4,
  STUCK_COLOR = 5,
};

void conch_init_colors();

#endif /* _COLORS_H */
