#ifndef _TIMEOUT_H
#define _TIMEOUT_H

#include <time.h>
#include <stdbool.h>

typedef struct {
  time_t start;
  double delay;
} conch_timeout;

conch_timeout *conch_timeout_new(double delay);

void conch_timeout_reset(conch_timeout *t);
bool conch_timeout_expired(conch_timeout const *t);

void conch_timeout_free(conch_timeout *t);

#endif /* _TIMEOUT_H */
