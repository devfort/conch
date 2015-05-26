#ifndef _BLASTLIST_H
#define _BLASTLIST_H

#include "backend.h"

typedef struct blast blast;

struct blast {
  id id;
  char *user;
  char *content;
  char *posted_at;
  char *attachment;

  blast *prev;
  blast *next;
};

typedef struct blastlist {
  blast *head;
  blast *current;
  blast *tail;
} blastlist;

blastlist *conch_blastlist_new();
blastlist *conch_blastlist_from_resultset(resultset *rs);
blastlist *conch_blastlist_join(blastlist *lhs, blastlist *rhs);

void conch_blastlist_free(blastlist *bl);

#endif /* _BLASTLIST_H */
