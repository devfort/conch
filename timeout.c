#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#include "timeout.h"

conch_timeout *conch_timeout_new(double delay) {
  conch_timeout *t = calloc(1, sizeof(conch_timeout));
  t->start = time(NULL);
  t->delay = delay;
  return t;
}

void conch_timeout_reset(conch_timeout *t) {
  assert(t != NULL);

  t->start = time(NULL);
}

bool conch_timeout_expired(conch_timeout const *t) {
  assert(t != NULL);

  time_t now = time(NULL);
  double delta = difftime(now, t->start);

  return (delta > t->delay);
}

void conch_timeout_free(conch_timeout *t) { free(t); }
