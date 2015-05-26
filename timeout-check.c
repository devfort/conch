#include <unistd.h>

#include "checkrunner.h"

#include "timeout.h"

#define ASSERT_PTR_NULL(ptr) ck_assert_ptr_eq(ptr, NULL)
#define ASSERT_PTR_NOT_NULL(ptr) ck_assert_ptr_ne(ptr, NULL)

START_TEST(test_timeout_new) {
  conch_timeout *t = conch_timeout_new(1);

  ASSERT_PTR_NOT_NULL(t);

  conch_timeout_free(t);
}
END_TEST

START_TEST(test_timeout_expiry) {
  conch_timeout *t = conch_timeout_new(0.5);

  ck_assert(!conch_timeout_expired(t));
  sleep(1);
  ck_assert(conch_timeout_expired(t));

  conch_timeout_free(t);
}
END_TEST

START_TEST(test_timeout_reset) {
  conch_timeout *t = conch_timeout_new(0.5);

  ck_assert(!conch_timeout_expired(t));
  sleep(1);
  conch_timeout_reset(t);
  ck_assert(!conch_timeout_expired(t));

  conch_timeout_free(t);
}
END_TEST

Suite *timeout_suite(void) {
  Suite *s = suite_create("timeout");

  ADD_TEST_CASE(s, test_timeout_new);
  ADD_TEST_CASE(s, test_timeout_expiry);
  ADD_TEST_CASE(s, test_timeout_reset);

  return s;
}

CONCH_CHECK_MAIN(timeout_suite)
