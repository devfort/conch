#ifndef _CHECKRUNNER_H
#define _CHECKRUNNER_H

#include <check.h>

/*
 * Run a check suite. Example:
 *
 *   Suite *foo_suite(void);
 *
 *   conch_check_runsuite(foo_suite());
 *
 * Returns 1 on suite failures, 0 otherwise.
 */
int conch_check_runsuite(Suite *s) {
  int number_failed;
  SRunner *sr;

  sr = srunner_create(s);

  srunner_run_all(sr, CK_VERBOSE);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? 0 : 1;
}

#define CONCH_CHECK_MAIN(suite)                                                \
  int main(void) { return conch_check_runsuite(suite()); }

#endif
