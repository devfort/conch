#ifndef _BLASTLIST_H
#define _BLASTLIST_H

#include "conchbackend.h"

typedef struct blastlist_item blastlist_item;

struct blastlist_item {
  id id;
  char *user;
  char *content;

  blastlist_item *prev;
  blastlist_item *next;
};

typedef struct { blastlist_item *head; } blastlist;

blastlist *conch_blastlist_new(result_set *rs);

void conch_blastlist_insert(blastlist *bs, result_set *rs);
void conch_blastlist_free(blastlist *bs);

#endif
