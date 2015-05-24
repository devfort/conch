#ifndef _CONCH_BACKEND_H
#define _CONCH_BACKEND_H

#include <stdint.h>
#include <libpq-fe.h>

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
  PGConn *connection;
  settings settings; 
} mouthpiece;

mouthpiece *connect(settings settings);
void disconnect(mouthpiece *mp);

result_set *recent_blasts(mouthpiece *mp);
result_set *blasts_before(mouthpiece *mp, id before_token);
result_set *blasts_after(mouthpiece *mp, id after_token);

void free_result_set(result_set *results);

#endif
