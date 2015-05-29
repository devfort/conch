#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "detailview-thread.h"
#include "explode.h"

// Non threaded fallback version
void conch_detailview_fetchattachment(detailview *v) {
  v->attachment = conch_webfetcher_get(v->blastlist->current->attachment);

  // If we have an attachment and it's Content-Type from the server is
  // image/* then try to display it
  if (v->attachment &&
      strncmp(v->attachment->content_type, "image/", strlen("image/")) == 0) {
    v->anigif = anigif_new_from_blob(v->attachment->content,
                                     v->attachment->content_length);
  }
}

typedef struct {
  webfetch_result *attachment;
  anigif *anigif;
} fetch_thread_result;

void *fetch_func(void *p) {
  detailview *v = (detailview *)p;

  fetch_thread_result *result = calloc(1, sizeof(fetch_thread_result));

  result->attachment = conch_webfetcher_get(v->blastlist->current->attachment);

  // If we have an attachment and it's Content-Type from the server is
  // image/* then try to display it
  if (result->attachment &&
      strncmp(result->attachment->content_type, "image/", strlen("image/")) ==
          0) {
    result->anigif = anigif_new_from_blob(result->attachment->content,
                                          result->attachment->content_length);
  }

  return (void *)result;
}

void conch_detailview_spawnfetchthread(detailview *v) {
  pthread_attr_t attr;

  int err = pthread_attr_init(&attr);
  if (err != 0) {
    fatal_error("pthread_attr_init: %s", strerror(err));
  }

  err = pthread_create(&(v->fetch_thread), &attr, &fetch_func, (void *)v);
  if (err != 0) {
    fatal_error("pthread_create: %s", strerror(err));
  }

  err = pthread_attr_destroy(&attr);
  if (err != 0) {
    fatal_error("pthread_attr_destroy: %s", strerror(err));
  }
}

void conch_detailview_killfetchthread(detailview *v) {
  if (v->fetch_thread == 0)
    return;

  pthread_cancel(v->fetch_thread);

  fetch_thread_result *result;
  int err = pthread_join(v->fetch_thread, (void **)&result);
  // EINVAL will happen if something has killed our thread for us
  if (err != 0 && err != ESRCH) {
    fatal_error("conch_detailview_killfetchthread pthtead_join: %d %s", err,
                strerror(err));
  }
  v->fetch_thread = 0;

  if (result != NULL && result != PTHREAD_CANCELED) {
    // It could have returned something even though we asked to cancel. (I
    // think). Lets free if it did
    if (result->attachment) {
      conch_webfetch_result_free(result->attachment);
    }

    if (result->anigif) {
      anigif_free(result->anigif);
    }
    free(result);
  }
}

bool conch_detailview_pollfetchthread(detailview *v) {

  if (v->fetch_thread == 0) {
    // Already finished!
    return false;
  }

  // much like `kill $pid 0` we can pthread_kill to see if the thread is still
  // alive. If it is it will return 0. If it's finished (or been killed) it
  // will return an error
  int err = pthread_kill(v->fetch_thread, 0);

  if (err == 0) {
    return false;
  }

  fetch_thread_result *result = NULL;
  err = pthread_join(v->fetch_thread, (void **)&result);
  // ESRCH can happen if the thread has been killed by something. It's not
  // fatal so deal with it
  if (err != 0 && err != ESRCH && err != EINVAL) {
    fatal_error("conch_detailview_pollfetchthread pthtead_join: %s",
                strerror(err));
  }

  v->fetch_thread = 0;

  if (result && result != PTHREAD_CANCELED) {
    v->attachment = result->attachment;
    v->anigif = result->anigif;

    free(result);
  }

  return true;
}
