#ifndef _KEYS_H
#define _KEYS_H

#include "render.h"

#include "listview.h"
#include "detailview.h"

// Maximum time to wait for a keypress (tenths of a second)
#define KEY_DELAY 1

typedef enum {
  CONCH_NOP,
  CONCH_CONCH,
  CONCH_EXIT_DETAIL,
  CONCH_DETAIL,
  CONCH_EXIT,
  CONCH_LIST,
} keypress_result;

keypress_result conch_keypress_dispatch(const int key, view_type current_view,
                                        void *view_state);
void conch_default_input_config();
void conch_getstr_input_config();

#endif /* _KEYS_H */
