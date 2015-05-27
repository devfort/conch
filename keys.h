#ifndef _KEYS_H
#define _KEYS_H

#include "render.h"

#include "listview.h"

typedef enum {
  CONCH_NOP,
  CONCH_EXIT,
} keypress_result;

keypress_result conch_keypress_dispatch(const int key, view_type current_view,
                                        void *view_state);

#endif /* _KEYS_H */
