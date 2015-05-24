#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "blastlist.h"

static char *strclone(char *c) {
  char *target = malloc(strlen(c) + 1);

  if(target == NULL) {
    fprintf(stderr, "strclone: could not alloc\n");
    abort();
  }

  strcpy(target, c);
  return target;
}

static blastlist_item *conch_blastlist_item_new(blast *b) {
  blastlist_item *it = (blastlist_item *)calloc(1, sizeof(blastlist_item));

  if(it == NULL) {
    fprintf(stderr, "conch_blastlist_item_new: could not alloc\n");
    abort();
  }

  it->id = b->id;
  it->user = strclone(b->user);
  it->content = strclone(b->content);

  return it;
}

static void conch_blastlist_item_free(blastlist_item *it) {
  free(it->user);
  free(it->content);
  free(it);
}

blastlist *conch_blastlist_new(result_set *rs) {
  blastlist *bl = (blastlist *)calloc(1, sizeof(blastlist));

  if(bl == NULL) {
    fprintf(stderr, "conch_blastlist_new: could not alloc\n");
    abort();
  }

  // If there are no items in the resultset, return an empty blastlist.
  if(rs->count < 1) {
    return bl;
  }

  // Otherwise, we append to the linked list
  bl->head = conch_blastlist_item_new(rs->blasts);
  blastlist_item *cur = bl->head;

  for(uint64_t i = 1; i < rs->count; ++i) {
    cur->next = conch_blastlist_item_new(rs->blasts + i);
    cur->next->prev = cur;
    cur = cur->next;
  }

  return bl;
}

void conch_blastlist_free(blastlist *bl) {
  blastlist_item *cur;
  blastlist_item *next = bl->head;

  while(next != NULL) {
    cur = next;
    next = cur->next;
    conch_blastlist_item_free(cur);
  }

  free(bl);
}
