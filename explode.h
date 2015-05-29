#ifndef __EXPLODE_H
#define __EXPLODE_H

#include <sys/cdefs.h>

#ifndef __printflike
#define __printflike(fmtarg, firstvararg)                                      \
  __attribute__((__format__(__printf__, fmtarg, firstvararg)))
#endif

void fatal_error(const char *fmt, ...) __printflike(1, 2);

#endif
