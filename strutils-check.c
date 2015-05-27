#include <unistd.h>

#include "checkrunner.h"

#include "strutils.h"

#define ASSERT_PTR_NULL(ptr) ck_assert_ptr_eq(ptr, NULL)
#define ASSERT_PTR_NOT_NULL(ptr) ck_assert_ptr_ne(ptr, NULL)

START_TEST(test_count_lines_and_find_length_of_longest) {
  const char *const input = "line 1\n"
                            "much longer line 2\n"
                            "\n"
                            "s3\n";

  int longest;
  int num = count_lines_and_find_length_of_longest(input, &longest);

  ck_assert_int_eq(num, 5);
  ck_assert_int_eq(longest, strlen("much longer line 2"));
}
END_TEST

START_TEST(test_count_lines_and_find_length_of_longest_with_one_line) {
  const char *const input = "line 1";

  int longest;
  int num = count_lines_and_find_length_of_longest(input, &longest);

  ck_assert_int_eq(num, 1);
  ck_assert_int_eq(longest, strlen("line 1"));
}
END_TEST

START_TEST(
    test_count_lines_and_find_length_of_longest_with_one_line_with_trailing_new_line) {
  const char *const input = "line 1\n";

  int longest;
  int num = count_lines_and_find_length_of_longest(input, &longest);

  ck_assert_int_eq(num, 2);
  ck_assert_int_eq(longest, strlen("line 1"));
}
END_TEST

START_TEST(test_no_wrap_when_text_fits_on_one_line) {
  char *expected_blast_lines[1024];
  char **actual_blast_lines;

  int available_width = 42;

  expected_blast_lines[0] = "We're going to need a bigger boat";
  expected_blast_lines[1] = NULL;

  actual_blast_lines =
      wrap_lines("We're going to need a bigger boat", available_width);
  for (int i = 0; expected_blast_lines[i]; i++) {
    ck_assert_str_eq(expected_blast_lines[i], actual_blast_lines[i]);
  }

  wrap_lines_free(actual_blast_lines);
}
END_TEST

START_TEST(test_lines_wrap_at_13) {
  char *expected_blast_lines[1024];
  char **actual_blast_lines;

  int available_width = 13;

  expected_blast_lines[0] = "We're going";
  expected_blast_lines[1] = "to need a";
  expected_blast_lines[2] = "bigger boat";
  expected_blast_lines[3] = NULL;

  actual_blast_lines =
      wrap_lines("We're going to need a bigger boat", available_width);
  for (int i = 0; expected_blast_lines[i]; i++) {
    ck_assert_str_eq(expected_blast_lines[i], actual_blast_lines[i]);
  }

  wrap_lines_free(actual_blast_lines);
}
END_TEST

START_TEST(test_lines_wrap_at_30) {
  char *expected_blast_lines[1024];
  char **actual_blast_lines;

  int available_width = 30;

  expected_blast_lines[0] = "We're going to need a bigger";
  expected_blast_lines[1] = "boat";
  expected_blast_lines[2] = NULL;

  actual_blast_lines =
      wrap_lines("We're going to need a bigger boat", available_width);
  for (int i = 0; expected_blast_lines[i]; i++) {
    ck_assert_str_eq(expected_blast_lines[i], actual_blast_lines[i]);
  }

  wrap_lines_free(actual_blast_lines);
}
END_TEST

START_TEST(test_wrap_lines_with_multiple_spaces) {
  char *expected_blast_lines[1024];
  char **actual_blast_lines;

  int available_width = 14;

  expected_blast_lines[0] = "We're going to";
  expected_blast_lines[1] = "need a bigger";
  expected_blast_lines[2] = "boat";
  expected_blast_lines[3] = NULL;

  actual_blast_lines =
      wrap_lines("We're going to    need a bigger boat", available_width);
  for (int i = 0; expected_blast_lines[i]; i++) {
    ck_assert_str_eq(expected_blast_lines[i], actual_blast_lines[i]);
  }

  wrap_lines_free(actual_blast_lines);
}
END_TEST

// probably ideally it would trim a trailing space
// but for the purposes of this, it doesn't really matter
START_TEST(test_wrap_lines_lands_on_a_space) {
  char *expected_blast_lines[1024];
  char **actual_blast_lines;

  int available_width = 15;

  expected_blast_lines[0] = "We're going to "; // note the trailing space
  expected_blast_lines[1] = "need a bigger";
  expected_blast_lines[2] = "boat";
  expected_blast_lines[3] = NULL;

  actual_blast_lines =
      wrap_lines("We're going to    need a bigger boat", available_width);
  for (int i = 0; expected_blast_lines[i]; i++) {
    ck_assert_str_eq(expected_blast_lines[i], actual_blast_lines[i]);
  }

  wrap_lines_free(actual_blast_lines);
}
END_TEST

START_TEST(test_wrap_lines_with_no_spaces) {
  char *expected_blast_lines[1024];
  char **actual_blast_lines;

  expected_blast_lines[0] = "You'rego";
  expected_blast_lines[1] = "ingtonee";
  expected_blast_lines[2] = "dabigger";
  expected_blast_lines[3] = "boat";
  expected_blast_lines[4] = NULL;

  int available_width = 8;

  actual_blast_lines =
      wrap_lines("You'regoingtoneedabiggerboat", available_width);
  for (int i = 0; expected_blast_lines[i]; i++) {
    ck_assert_str_eq(expected_blast_lines[i], actual_blast_lines[i]);
  }

  wrap_lines_free(actual_blast_lines);
}
END_TEST

// We can't accurately test wrapping output because rendering
// depends on where the string is broken, so just check nothing explodes
START_TEST(test_wrap_lines_with_odd_unicode_doesnt_explode) {
  char **actual_blast_lines;

  int available_width = 13;

  actual_blast_lines =
      wrap_lines("H̕͝ ̛G̢̧̛Ó̧̕D̶͘ ̢ST̶͢ÓP T͠H̷͏E̵̸̛͝ "
                 "̶́͝P̷͡Ą͠͠͡I̷͟N̶͜!¡!",
                 available_width);

  wrap_lines_free(actual_blast_lines);
}
END_TEST

START_TEST(test_wrap_lines_empty_text) {
  char *expected_blast_lines[1024];
  char **actual_blast_lines;

  int available_width = 15;

  expected_blast_lines[0] = "";
  expected_blast_lines[1] = NULL;

  actual_blast_lines = wrap_lines("", available_width);
  for (int i = 0; expected_blast_lines[i]; i++) {
    ck_assert_str_eq(expected_blast_lines[i], actual_blast_lines[i]);
  }

  wrap_lines_free(actual_blast_lines);
}
END_TEST

START_TEST(test_wrap_lines_null_text) {
  char *expected_blast_lines[1024];
  char **actual_blast_lines;

  int available_width = 15;

  expected_blast_lines[0] = "";
  expected_blast_lines[1] = NULL;

  actual_blast_lines = wrap_lines(NULL, available_width);
  for (int i = 0; expected_blast_lines[i]; i++) {
    ck_assert_str_eq(expected_blast_lines[i], actual_blast_lines[i]);
  }

  wrap_lines_free(actual_blast_lines);
}
END_TEST

START_TEST(test_wrap_lines_free) {
  char **actual_blast_lines = NULL;
  wrap_lines_free(actual_blast_lines);
}
END_TEST

Suite *strutils_suite(void) {
  Suite *s = suite_create("strutils");

  ADD_TEST_CASE(s, test_count_lines_and_find_length_of_longest);
  ADD_TEST_CASE(s, test_count_lines_and_find_length_of_longest_with_one_line);
  ADD_TEST_CASE(
      s,
      test_count_lines_and_find_length_of_longest_with_one_line_with_trailing_new_line);
  ADD_TEST_CASE(s, test_no_wrap_when_text_fits_on_one_line);
  ADD_TEST_CASE(s, test_lines_wrap_at_13);
  ADD_TEST_CASE(s, test_lines_wrap_at_30);
  ADD_TEST_CASE(s, test_wrap_lines_with_multiple_spaces);
  ADD_TEST_CASE(s, test_wrap_lines_lands_on_a_space);
  ADD_TEST_CASE(s, test_wrap_lines_empty_text);
  ADD_TEST_CASE(s, test_wrap_lines_null_text);
  ADD_TEST_CASE(s, test_wrap_lines_free);
  ADD_TEST_CASE(s, test_wrap_lines_with_no_spaces);
  ADD_TEST_CASE(s, test_wrap_lines_with_odd_unicode_doesnt_explode);

  return s;
}

CONCH_CHECK_MAIN(strutils_suite)
