#ifndef _CONCH_BACKEND_H
#define _CONCH_BACKEND_H

#include <stdint.h>

typedef uint64_t id;

typedef struct {
  id id;
  char *user;
  char *content;
} blast;

typedef struct {
  // Invariant: If count > 0 then error == 0
  // If != 0, the call has failed. When we know more there will be useful
  // error codes here...
  int error;
  // Tokens to pass to paging.
  id before_token;
  id after_token;
  // Number of blasts then a pointer to that many valid blasts.
  // Invariant: If count == 0 then blasts == NULL
  uint64_t count;
  blast *blasts;
} result_set;

typedef struct { uint16_t page_size; } settings;

typedef struct mouthpiece mouthpiece;

mouthpiece *conch_connect(settings settings);
void conch_disconnect(mouthpiece *mp);

result_set *conch_recent_blasts(mouthpiece *mp);
result_set *conch_blasts_before(mouthpiece *mp, id before_token);
result_set *conch_blasts_after(mouthpiece *mp, id after_token);

void conch_free_result_set(result_set *results);

#endif
