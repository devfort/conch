#ifndef _KEYS_H
#define _KEYS_H

#include "listview.h"

typedef enum {
  CONCH_NOP,
  CONCH_EXIT,
} keypress_result;

keypress_result conch_keypress_dispatch(const int key, listview *lv);

#endif /* _KEYS_H */
