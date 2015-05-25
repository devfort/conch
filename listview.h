#ifndef _LISTVIEW_H
#define _LISTVIEW_H

#include "blastlist.h"

typedef struct screen_state_s {
  blastlist *head;
  blastlist *current_blast;
  int blast_offset;
  int stick_to_top;
} screen_state_s;

screen_state_s *conch_listview_new(blastlist *bl, int stick_to_top);
void conch_listview_free(screen_state_s *lv);

#endif /* _LISTVIEW_H */
