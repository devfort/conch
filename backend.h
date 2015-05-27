#ifndef _BACKEND_H
#define _BACKEND_H

#include <stdint.h>
#include <stdbool.h>

typedef uint64_t id;
#define PRIid PRIu64

typedef struct {
  id id;
  char *user;
  char *content;
  char *posted_at;
  char *attachment;
  char *extended;
} blastdata;

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
  blastdata *blasts;
} resultset;

typedef struct {
  uint16_t page_size;
  char *host;
  char *database;
} settings;

typedef struct mouthpiece mouthpiece;

mouthpiece *conch_connect(settings settings);
void conch_disconnect(mouthpiece *mp);

resultset *conch_recent_blasts(mouthpiece *mp);
resultset *conch_blasts_before(mouthpiece *mp, id before_token);
resultset *conch_blasts_after(mouthpiece *mp, id after_token);

typedef struct {
  // id == 0 indicates an error and will result in a non-null error_message
  id post;
  char *error_message;
} blastresult;

blastresult *conch_blast_post(mouthpiece *mp, char *user, char *content,
                              char *extended);

void conch_resultset_free(resultset *results);
void conch_blastresult_free(blastresult *result);

typedef struct {
  uint64_t last_notify;
  mouthpiece *mouthpiece;
} notifications;

void conch_notifications_init(notifications *notifications, mouthpiece *mp);
bool conch_notifications_poll(notifications *notifications);
bool conch_notifications_await(notifications *notifications, int timeout_ms);

#endif /* _BACKEND_H */
