#include <curses.h>
#include <stdlib.h>

#include "listview.h"

void conch_keypress_dispatch(const int key, listview *lv) {
  switch (key) {
  case '0':
    conch_listview_jump_to_top(lv);
    break;

  case '.':
  case '\t':
    conch_listview_jump_to_next_unread(lv);
    break;

  case 'j':
    conch_listview_select_next_blast(lv);
    break;

  case 'k':
    conch_listview_select_prev_blast(lv);
    break;

  case 's':
    conch_listview_toggle_stick_to_top(lv);
    break;

  case 'q':
    endwin();
    exit(0);
    break;

  case ERR:
  default:
    break;
  }
}
