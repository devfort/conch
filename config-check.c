#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "config.h"
#include "backend.h"

#include "checkrunner.h"

START_TEST(test_load_full_config) {
  settings config = conch_load_config("rsrc/config/full.lua");
  ck_assert_ptr_ne(config.username, NULL);
  ck_assert_str_eq(config.username, "subversion");
  ck_assert_ptr_ne(config.host, NULL);
  ck_assert_str_eq(config.host, "localhost");
  ck_assert_ptr_ne(config.database, NULL);
  ck_assert_str_eq(config.database, "bugle_test");
}
END_TEST

START_TEST(test_load_empty_config) {
  settings config = conch_load_config("rsrc/config/empty.lua");
  ck_assert_ptr_eq(config.username, NULL);
  ck_assert_ptr_eq(config.host, NULL);
  ck_assert_ptr_eq(config.database, NULL);
}
END_TEST

START_TEST(test_load_missing_config) {
  settings config = conch_load_config("rsrc/config/nope.lua");
  ck_assert_ptr_eq(config.username, NULL);
  ck_assert_ptr_eq(config.host, NULL);
  ck_assert_ptr_eq(config.database, NULL);
}
END_TEST

START_TEST(test_unreadable_config) {
  char *unreadable = "rsrc/config/no-read.lua";
  // this file should not have read permission
  creat(unreadable, O_CREAT | O_TRUNC | S_IWUSR);

  settings config = conch_load_config(unreadable);
  ck_assert_ptr_eq(config.username, NULL);
}
END_TEST

Suite *config_suite(void) {
  Suite *s = suite_create("config");

  ADD_TEST_CASE(s, test_load_full_config);
  ADD_TEST_CASE(s, test_load_empty_config);
  ADD_TEST_CASE(s, test_load_missing_config);
  ADD_TEST_CASE(s, test_unreadable_config);

  return s;
}

CONCH_CHECK_MAIN(config_suite)
