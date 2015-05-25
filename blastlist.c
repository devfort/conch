#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "blastlist.h"
#include "strutils.h"

static void item_copy(blastlist *bl, blast *b) {
  bl->id = b->id;
  bl->user = strclone(b->user);
  bl->content = strclone(b->content);
  bl->posted_at = strclone(b->posted_at);
}

static void item_free(blastlist *bl) {
  free(bl->user);
  free(bl->content);
  free(bl->posted_at);
  free(bl);
}

blastlist *conch_blastlist_new() {
  blastlist *bl = (blastlist *)calloc(1, sizeof(blastlist));

  if (bl == NULL) {
    fprintf(stderr, "conch_blastlist_new: could not alloc\n");
    abort();
  }

  return bl;
}

blastlist *conch_blastlist_from_result_set(result_set *rs) {
  // If there are no items out of which to make a blastlist, return NULL
  if (rs == NULL || rs->count == 0) {
    return NULL;
  }

  blastlist *head = conch_blastlist_new();
  item_copy(head, rs->blasts);

  for (uint64_t i = 1; i < rs->count; ++i) {
    blastlist *next = conch_blastlist_new();
    item_copy(next, rs->blasts + i);
    head = conch_blastlist_join(head, next);
  }

  return head;
}

blastlist *conch_blastlist_join(blastlist *lhs, blastlist *rhs) {
  if (lhs == NULL) {
    return rhs;
  }
  if (rhs == NULL) {
    return lhs;
  }

  // Otherwise both lists are non-null, so we find the tail of the lhs:
  blastlist *cur = lhs;
  while (1) {
    if (cur->next == NULL) {
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

  if (bl == NULL) {
    return;
  }

  // We should have received the head of a list, or I'm going to explode.
  assert(next->prev == NULL);

  while (next != NULL) {
    cur = next;
    next = cur->next;
    item_free(cur);
  }
}
