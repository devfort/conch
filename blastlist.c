#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "blastlist.h"

static char *strclone(char *c) {
  if(c == NULL) {
    return NULL;
  }

  char *target = malloc(strlen(c) + 1);

  if(target == NULL) {
    fprintf(stderr, "strclone: could not alloc\n");
    abort();
  }

  strcpy(target, c);
  return target;
}

static blastlist *item_new(blast *b) {
  blastlist *bl = (blastlist *)calloc(1, sizeof(blastlist));

  if(bl == NULL) {
    fprintf(stderr, "item_new: could not alloc\n");
    abort();
  }

  bl->id = b->id;
  bl->user = strclone(b->user);
  bl->content = strclone(b->content);

  return bl;
}

static void item_free(blastlist *bl) {
  free(bl->user);
  free(bl->content);
  free(bl);
}

blastlist *conch_blastlist_new(result_set *rs) {
  // If there are no items out of which to make a blastlist, return NULL
  if(rs == NULL || rs->count == 0) {
    return NULL;
  }

  blastlist *head = item_new(rs->blasts);
  blastlist *cur = head;

  for(uint64_t i = 1; i < rs->count; ++i) {
    cur->next = item_new(rs->blasts + i);
    cur->next->prev = cur;
    cur = cur->next;
  }

  return head;
}

blastlist *conch_blastlist_join(blastlist *lhs, blastlist *rhs) {
  if(lhs == NULL) {
    return rhs;
  }
  if(rhs == NULL) {
    return lhs;
  }

  // Otherwise both lists are non-null, so we find the tail of the lhs:
  blastlist *cur = lhs;
  while(1) {
    if(cur->next == NULL) {
      break;
    }
    cur = cur->next;
  }

  // And point the tail of lhs at the head of rhs:
  cur->next = rhs;
  cur->next->prev = cur;

  return lhs;
}

void conch_blastlist_free(blastlist *bl) {
  blastlist *cur;
  blastlist *next = bl;

  if(bl == NULL) {
    return;
  }

  // We should have received the head of a list, or I'm going to explode.
  assert(next->prev == NULL);

  while(next != NULL) {
    cur = next;
    next = cur->next;
    item_free(cur);
  }
}
