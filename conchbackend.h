#ifndef _CONCH_BACKEND_H
#define _CONCH_BACKEND_H

#include <stdint.h>
#include <postgresql/libpq-fe.h>

typedef uint64_t id;

typedef struct {
  id id;
  char *name;
} user;

typedef struct {
  id id;
  user *user;
  char *content;
} blast;

typedef struct {
  // Invariant: If count > 0 then error == 0
  int error;
  id before_token;
  id after_token;
  uint64_t count;
  blast *blasts;
} result_set;

typedef struct {
  uint16_t page_size;
} settings;

typedef struct {
  PGconn *connection;
  settings settings;
} mouthpiece;

mouthpiece *conch_connect(settings settings);
void conch_disconnect(mouthpiece *mp);

result_set *conch_recent_blasts(mouthpiece *mp);
result_set *conch_blasts_before(mouthpiece *mp, id before_token);
result_set *conch_blasts_after(mouthpiece *mp, id after_token);

void conch_free_result_set(result_set *results);

#endif
