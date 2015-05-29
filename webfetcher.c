#include "explode.h"
#include "strutils.h"
#include "webfetcher.h"

#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>

void conch_webfetcher_init() { curl_global_init(CURL_GLOBAL_ALL); }

void conch_webfetcher_cleanup() { curl_global_cleanup(); }

// This is a callback function called by curl when it has read a chunk of data.
// Curl needs us to return the number of bytes dealt with and if it doesn't
// match the input then our call to `curl_easy_perform` will return a
// CURLE_WRITE_ERROR
static size_t webfetcher_on_data(void *buffer, size_t size, size_t nmemb,
                                 void *stream) {
  webfetch_result *result = (webfetch_result *)stream;
  if (!result) {
    // Can't write!
    return -1;
  }

  size_t nbytes = size * nmemb;

  if (!result->content) {
    result->content = malloc(nbytes + 1);
  } else {
    result->content =
        realloc(result->content, result->content_length + nbytes + 1);
  }

  if (!result->content) {
    return -1;
  }

  // You can't do arthimentic on a void*, so cast the pointer to char* so we
  // can move it on
  memcpy((char *)result->content + result->content_length, buffer, nbytes);

  result->content_length += nbytes;
  return nbytes;
}

webfetch_result *conch_webfetcher_get(const char *url) {
  CURLcode res;
  char errbuf[CURL_ERROR_SIZE] = { '\0' };

  CURL *handle = curl_easy_init();

  if (handle == NULL) {
    fatal_error("conch_webfetcher_get: curl_easy_init failed!");
  }

  res = curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, errbuf);
  if (res != CURLE_OK) {
    // If we cant set the error buff then we can't get useful errors, just
    // abort!
    fatal_error(
        "conch_webfetcher_get: curl_easy_setopt(CURLOPT_ERRORBUFFER) failed!");
  }

  webfetch_result *result = calloc(1, sizeof(webfetch_result));
  result->success = false;

  /* Define our callback to get called when there's data to be written */
  curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, webfetcher_on_data);
  /* Set a pointer to our struct to pass to the callback */
  curl_easy_setopt(handle, CURLOPT_WRITEDATA, result);
  res = curl_easy_setopt(handle, CURLOPT_URL, url);

  if (res != CURLE_OK) {
    result->error = strclone(errbuf);
    curl_easy_cleanup(handle);
    return result;
  }

  res = curl_easy_perform(handle);
  if (res != CURLE_OK) {
    result->error = strclone(errbuf);
    curl_easy_cleanup(handle);
    return result;
  }

  char *content_type;
  res = curl_easy_getinfo(handle, CURLINFO_CONTENT_TYPE, &content_type);
  if (res == CURLE_OK && content_type) {
    result->content_type = strclone(content_type);
  }

  result->success = true;
  curl_easy_cleanup(handle);
  return result;
}

void conch_webfetch_result_free(webfetch_result *result) {
  if (result) {
    free(result->error);
    free(result->content);
  }
  free(result);
}
