#include "keys.h"
#include "listview.h"
#include "detailview.h"

#include "listview-keys.h"
#include "detailview-keys.h"

keypress_result conch_keypress_dispatch(const int key, view_type current_view,
                                        void *view_state) {
  switch (key) {

  case 'q':
    return CONCH_EXIT;
  case '@':
    // Toggle "conch view"
    if (current_view == VIEW_CONCH) {
      return CONCH_LIST;
    } else {
      return CONCH_CONCH;
    }

  case -1:
    break;
  default:
    switch (current_view) {
    case VIEW_LIST:
      return conch_listview_keypress_dispatch(key, (listview *)view_state);
    case VIEW_DETAIL:
      return conch_detailview_keypress_dispatch(key, (detailview *)view_state);
    case VIEW_CONCH:
      break;
    }
  }

  return CONCH_NOP;
}

void conch_default_input_config() {
  // Disable line buffering
  cbreak();

  // Turn terminal local keypress echo off
  noecho();

  // Ask ncurses to give us KEY_LEFT etc, not escape followed by some more keys
  keypad(curscr, true);

  // Turn on "half delay" mode, in which getch functions will block for up to n
  // tenths of a second before returning ERR.
  halfdelay(KEY_DELAY);

  // Hide the cursor
  const int cursor_invisible = 0;
  curs_set(cursor_invisible);
}

void conch_getstr_input_config() {
  // Turn local echo back on
  echo();

  // Make the input cursor visible
  curs_set(1);

  // Show the cursor again
  const int cursor_visible = 1;
  curs_set(cursor_visible);
}
