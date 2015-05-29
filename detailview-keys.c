#include "detailview-keys.h"

#include <curses.h>

extern void fatal_error(const char *, ...);

keypress_result conch_detailview_keypress_dispatch(int key, detailview *v) {

  switch (key) {
  case 'j':
  case KEY_DOWN:
    conch_detailview_scroll_down(v);
    break;
  case 'k':
  case KEY_UP:
    conch_detailview_scroll_up(v);
    break;
  case 'h':
  case KEY_LEFT:
    conch_detailview_scroll_code_left(v);
    break;
  case 'l':
  case KEY_RIGHT:
    conch_detailview_scroll_code_right(v);
    break;
  case '\r':
  case '\n':
    return CONCH_LIST;
  }
  return CONCH_NOP;
}
