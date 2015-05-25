#ifndef _BLASTLIST_H
#define _BLASTLIST_H

#include "backend.h"

typedef struct blastlist blastlist;

struct blastlist {
  id id;
  char *user;
  char *content;
  char *posted_at;
  char *attachment;

  blastlist *prev;
  blastlist *next;
};

blastlist *conch_blastlist_new();
blastlist *conch_blastlist_from_resultset(resultset *rs);
blastlist *conch_blastlist_join(blastlist *lhs, blastlist *rhs);

void conch_blastlist_free(blastlist *bl);

#endif /* _BLASTLIST_H */
