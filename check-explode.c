#include <setjmp.h>

extern jmp_buf check_jump;

void fatal_error(const char *fmt, ...) {
  longjmp(check_jump, 1);
}
