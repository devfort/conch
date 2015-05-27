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

  ck_assert_int_eq(num, 4);
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

  ck_assert_int_eq(num, 1);
  ck_assert_int_eq(longest, strlen("line 1"));
}
END_TEST
Suite *strutils_suite(void) {
  Suite *s = suite_create("strutils");

  ADD_TEST_CASE(s, test_count_lines_and_find_length_of_longest);
  ADD_TEST_CASE(s, test_count_lines_and_find_length_of_longest_with_one_line);
  ADD_TEST_CASE(
      s,
      test_count_lines_and_find_length_of_longest_with_one_line_with_trailing_new_line);

  return s;
}

CONCH_CHECK_MAIN(strutils_suite)
