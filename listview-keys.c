#include "listview-keys.h"

#include <curses.h>

extern void fatal_error(const char *, ...);

keypress_result conch_listview_keypress_dispatch(int key, listview *lv) {

  switch (key) {
  case '0':
  case 'g':
    conch_listview_jump_to_top(lv);
    break;
  case 'G':
    conch_listview_jump_to_bottom(lv);
    break;
  case '.':
  case '\t':
    conch_listview_jump_to_next_unread(lv);
    break;

  case KEY_DOWN:
  case 'j':
    conch_listview_select_next_blast(lv);
    break;

  case KEY_UP:
  case 'k':
    conch_listview_select_prev_blast(lv);
    break;

  case KEY_NPAGE:
  case ' ':
    conch_listview_page_down(lv);
    break;

  case KEY_PPAGE:
    conch_listview_page_up(lv);
    break;

  case 's':
    conch_listview_toggle_stick_to_top(lv);
    break;

  case '/':
    conch_listview_search_forward(lv);
    break;

  case 'n':
    conch_listview_repeat_search_forward(lv);
    break;

  case '\r':
  case '\n':
    return CONCH_DETAIL;
  }
  return CONCH_NOP;
}
