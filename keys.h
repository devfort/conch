#ifndef _KEYS_H
#define _KEYS_H

#include "render.h"

#include "listview.h"
#include "detailview.h"

typedef enum {
  CONCH_NOP,
  CONCH_CONCH,
  CONCH_DETAIL,
  CONCH_EXIT,
  CONCH_LIST,
} keypress_result;

keypress_result conch_keypress_dispatch(const int key, view_type current_view,
                                        void *view_state);

#endif /* _KEYS_H */
