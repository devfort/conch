#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <setjmp.h>

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
  bool caught = false;
  char *unreadable = "rsrc/config/no-read.lua";
  // this file should not have read permission
  creat(unreadable, O_CREAT | O_TRUNC | S_IWUSR);

  if (setjmp(check_jump) == 0) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
    settings config = conch_load_config(unreadable);
    config.username = "using the username";
#pragma GCC diagnostic pop
  } else {
    caught = true;
  }

  ck_assert_msg(caught, "fatal error was not called");
}
END_TEST

START_TEST(test_clock_function_error) {
  char *explode = "rsrc/config/explode.lua";
  char clock_buffer[100];
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
  settings config = conch_load_config(explode);
  config.username = "using the username";
#pragma GCC diagnostic pop
  generate_clock_text(100, clock_buffer);
  ck_assert_msg(true, "generate_clock_text should not crash");
}
END_TEST

Suite *config_suite(void) {
  Suite *s = suite_create("config");

  ADD_TEST_CASE(s, test_load_full_config);
  ADD_TEST_CASE(s, test_load_empty_config);
  ADD_TEST_CASE(s, test_load_missing_config);
  ADD_TEST_CASE(s, test_unreadable_config);
  ADD_TEST_CASE(s, test_clock_function_error);

  return s;
}

CONCH_CHECK_MAIN(config_suite)
