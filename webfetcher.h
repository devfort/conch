#ifndef _WEBFETCHER_H
#define _WEBFETCHER_H

#include <stdbool.h>
#include <stdlib.h>

typedef struct {
  bool success;
  char *error;
  void *content;
  char *content_type;
  size_t content_length;
} webfetch_result;

void conch_webfetcher_init();
void conch_webfetcher_cleanup();

webfetch_result *conch_webfetcher_get(const char *url);

void conch_webfetch_result_free(webfetch_result *result);

#endif
