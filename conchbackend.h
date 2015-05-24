#ifndef _CONCH_BACKEND_H
#define _CONCH_BACKEND_H

#include <stdint.h>

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
  uint64_t count;
  blast *blasts;
} result_set;

typedef struct {} mouthpiece;

typedef struct {
  uint16_t page_size;
} settings;

mouthpiece *connect(settings settings);
void disconnect(mouthpiece *mp);

result_set *recent_blasts(mouthpiece *mp);
result_set *blasts_before(mouthpiece *mp, id before_id);

void free_result_set(result_set *results);

#endif
