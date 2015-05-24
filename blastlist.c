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

static blastlist_item *ll_from_resultset(result_set *rs) {
  // If there are no items in the resultset, return NULL;
  if(rs->count < 1) {
    return NULL;
  }

  blastlist_item *head = conch_blastlist_item_new(rs->blasts);
  blastlist_item *cur = head;

  for(uint64_t i = 1; i < rs->count; ++i) {
    cur->next = conch_blastlist_item_new(rs->blasts + i);
    cur->next->prev = cur;
    cur = cur->next;
  }

  return head;
}

blastlist *conch_blastlist_new(result_set *rs) {
  blastlist *bl = (blastlist *)calloc(1, sizeof(blastlist));

  if(bl == NULL) {
    fprintf(stderr, "conch_blastlist_new: could not alloc\n");
    abort();
  }

  bl->head = ll_from_resultset(rs);

  return bl;
}

void conch_blastlist_insert(blastlist *bl, result_set *rs) {
  blastlist_item *new = ll_from_resultset(rs);

  if(new == NULL) {
    return;
  }

  // Prepend the new linked list to the existing one by first finding the tail
  // of the new linked list...
  blastlist_item *cur = new;
  while(1) {
    if(cur->next == NULL) {
      break;
    }
    cur = cur->next;
  }

  // ...pointing the tail at the current head...
  if(bl->head != NULL) {
    cur->next = bl->head;
    cur->next->prev = cur;
  }

  // ...and update the head pointer.
  bl->head = new;
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
