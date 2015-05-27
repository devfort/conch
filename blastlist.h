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
void conch_blastlist_prepend_resultset(blastlist *bl, resultset *rs);
void conch_blastlist_append_resultset(blastlist *bl, resultset *rs);

void conch_blastlist_free(blastlist *bl);

#endif /* _BLASTLIST_H */
