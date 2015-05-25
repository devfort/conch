#ifndef _BLASTLIST_H
#define _BLASTLIST_H

#include "conchbackend.h"

typedef struct blastlist blastlist;

struct blastlist {
  id id;
  char *user;
  char *content;

  blastlist *prev;
  blastlist *next;
};

blastlist *conch_blastlist_new(result_set *rs);
blastlist *conch_blastlist_join(blastlist *lhs, blastlist *rhs);

void conch_blastlist_free(blastlist *bl);

#endif
