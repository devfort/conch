#ifndef _BACKEND_H
#define _BACKEND_H

#include <stdint.h>

typedef uint64_t id;
#define PRIid PRIu64

typedef struct {
  id id;
  char *user;
  char *content;
  char *posted_at;
  char *attachment;
} blast;

typedef struct {
  // Invariant: If count > 0 then error == 0
  // If != 0, the call has failed. When we know more there will be useful
  // error codes here...
  int error;
  // String explanation of what went wrong.
  // Invariant: (error == 0) == (error_message == NULL)
  char *error_message;
  // Tokens to pass to paging.
  id before_token;
  id after_token;
  // Number of blasts then a pointer to that many valid blasts.
  // Invariant: If count == 0 then blasts == NULL
  uint64_t count;
  blast *blasts;
} resultset;

typedef struct { uint16_t page_size; } settings;

typedef struct mouthpiece mouthpiece;

mouthpiece *conch_connect(settings settings);
void conch_disconnect(mouthpiece *mp);

resultset *conch_recent_blasts(mouthpiece *mp);
resultset *conch_blasts_before(mouthpiece *mp, id before_token);
resultset *conch_blasts_after(mouthpiece *mp, id after_token);

void conch_free_resultset(resultset *results);

#endif /* _BACKEND_H */
