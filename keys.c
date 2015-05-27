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
