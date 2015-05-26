#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "blastlist.h"
#include "strutils.h"

static void blastdata_copy(blast *b, blastdata *d) {
  b->id = d->id;
  b->user = strclone(d->user);
  b->content = strclone(d->content);
  b->posted_at = strclone(d->posted_at);
  b->attachment = strclone(d->attachment);
}

static blast *blast_new() {
  blast *b = calloc(1, sizeof(blast));

  if (b == NULL) {
    fprintf(stderr, "conch_blast_new: could not alloc\n");
    abort();
  }

  return b;
}

static void blast_free(blast *b) {
  free(b->user);
  free(b->content);
  free(b->posted_at);
  free(b->attachment);
  free(b);
}

blastlist *conch_blastlist_new() {
  blastlist *bl = calloc(1, sizeof(blastlist));

  if (bl == NULL) {
    fprintf(stderr, "conch_blastlist_new: could not alloc\n");
    abort();
  }

  return bl;
}

blastlist *conch_blastlist_from_resultset(resultset *rs) {
  blastlist *bl = conch_blastlist_new();

  // If there are no items out of which to make a blastlist, return
  if (rs == NULL || rs->count == 0) {
    return bl;
  }

  bl->head = blast_new();
  bl->current = bl->head;
  bl->tail = bl->head;
  blastdata_copy(bl->head, rs->blasts);

  blast *cur = bl->head;

  for (uint64_t i = 1; i < rs->count; ++i) {
    cur->next = blast_new();
    cur->next->prev = cur;
    blastdata_copy(cur->next, rs->blasts + i);
    bl->tail = cur;
    cur = cur->next;
  }

  return bl;
}

blastlist *conch_blastlist_join(blastlist *lhs, blastlist *rhs) {
  if (lhs == NULL) {
    return rhs;
  }
  if (rhs == NULL) {
    return lhs;
  }

  // Otherwise both lists are non-null, so we find the tail of the lhs:
  blast *cur = lhs->head;
  while (1) {
    if (cur->next == NULL) {
      break;
    }
    cur = cur->next;
  }

  // And point the tail of lhs at the head of rhs:
  cur->next = rhs->head;
  cur->next->prev = cur;

  lhs->tail = rhs->tail;

  return lhs;
}

void conch_blastlist_free(blastlist *bl) {
  if (bl == NULL) {
    return;
  }

  blast *cur;
  blast *next = bl->head;

  // We should have received the head of a list, or I'm going to explode.
  if (bl->head != NULL) {
    assert(next->prev == NULL);
  }

  while (next != NULL) {
    cur = next;
    next = cur->next;
    blast_free(cur);
  }

  free(bl);
}
