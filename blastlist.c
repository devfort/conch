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

static void blasts_from_resultset(resultset *rs, blast **head, blast **tail) {
  // If there are no items out of which to make a blastlist, return
  if (rs == NULL || rs->count == 0) {
    *head = NULL;
    *tail = NULL;
    return;
  }

  blast *cur = blast_new();
  blast *chainhead = cur;

  blastdata_copy(cur, rs->blasts);

  for (uint64_t i = 1; i < rs->count; ++i) {
    cur->next = blast_new();
    cur->next->prev = cur;
    blastdata_copy(cur->next, rs->blasts + i);
    cur = cur->next;
  }

  *head = chainhead;
  *tail = cur;
}

void conch_blastlist_prepend_resultset(blastlist *bl, resultset *rs) {
  blast *chainhead;
  blast *chaintail;

  if (rs == NULL) {
    return;
  }

  blasts_from_resultset(rs, &chainhead, &chaintail);

  // Nothing in resultset. We're done.
  if (chainhead == NULL || chaintail == NULL) {
    return;
  }

  if (bl->head == NULL) {
    bl->head = chainhead;
    bl->tail = chaintail;
    bl->current = bl->head;
    return;
  }

  bl->head->prev = chaintail;
  bl->head->prev->next = bl->head;
  bl->head = chainhead;
}

void conch_blastlist_append_resultset(blastlist *bl, resultset *rs) {
  blast *chainhead;
  blast *chaintail;

  blasts_from_resultset(rs, &chainhead, &chaintail);

  // Nothing in resultset. We're done.
  if (chainhead == NULL || chaintail == NULL) {
    return;
  }

  if (bl->tail == NULL) {
    bl->head = chainhead;
    bl->tail = chaintail;
    bl->current = bl->head;
    return;
  }

  bl->tail->next = chainhead;
  bl->tail->next->prev = bl->tail;
  bl->tail = chaintail;
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
