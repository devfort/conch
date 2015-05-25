#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "strutils.h"

char *strclone(char *c) {
  if (c == NULL) {
    return NULL;
  }

  char *target = malloc(strlen(c) + 1);

  if (target == NULL) {
    fprintf(stderr, "strclone: could not alloc\n");
    abort();
  }

  strcpy(target, c);
  return target;
}

char *strcopycat(char *c, char *d){
	assert(c != NULL);
	assert(d != NULL);

	int n = strlen(c);
	char *target = malloc(n + strlen(d) + 1);
	strcpy(target, c);
	strcpy(target + n, d);
	return target;
}
