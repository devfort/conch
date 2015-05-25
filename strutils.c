#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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
