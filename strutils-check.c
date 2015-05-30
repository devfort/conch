#include <stdlib.h>

#include <unistd.h>

#include "checkrunner.h"

#include "strutils.h"

#define ASSERT_STRARY_EQ(exp_len, act_len, exp_ary, act_ary)                   \
  ck_assert_int_eq(exp_len, act_len);                                          \
  for (unsigned int i = 0; i < exp_len; i++) {                                 \
    ck_assert_str_eq(exp_ary[i], act_ary[i]);                                  \
  }

START_TEST(test_strcopytrunc) {
  char const *const input = "Hello, world, I am far too long for the screen.";
  char *result;

  result = strcopytrunc(input, 12);

  ASSERT_PTR_NOT_NULL(result);
  ck_assert_int_eq(12, strlen(result));
  ck_assert_str_eq("Hello, world", result);

  free(result);
}
END_TEST

START_TEST(test_strcopytrunc_width_exceeds_strlen) {
  char const *const input = "Hello, world";
  char *result;

  result = strcopytrunc(input, 50);

  ASSERT_PTR_NOT_NULL(result);
  ck_assert_int_eq(12, strlen(result));
  ck_assert_str_eq("Hello, world", result);

  free(result);
}
END_TEST

START_TEST(test_strcopytrunc_zero) {
  char const *const input = "Hello, world, I am far too long for the screen.";
  char *result;

  result = strcopytrunc(input, 0);

  ASSERT_PTR_NOT_NULL(result);
  ck_assert_int_eq(0, strlen(result));
  ck_assert_str_eq("", result);

  free(result);
}
END_TEST

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
  unsigned int nout;
  char *expected[] = {
    "We're going to need a bigger boat",
  };

  char **actual = wrap_lines("We're going to need a bigger boat", 42, &nout);

  ASSERT_STRARY_EQ(1, nout, expected, actual);

  wrap_lines_free(actual, nout);
}
END_TEST

START_TEST(test_lines_wrap_at_13) {
  unsigned int nout;
  char *expected[] = {
    "We're going", "to need a", "bigger boat",
  };

  char **actual = wrap_lines("We're going to need a bigger boat", 13, &nout);

  ASSERT_STRARY_EQ(3, nout, expected, actual);

  wrap_lines_free(actual, nout);
}
END_TEST

START_TEST(test_lines_wrap_at_30) {
  unsigned int nout;
  char *expected[] = {
    "We're going to need a bigger", "boat",
  };

  char **actual = wrap_lines("We're going to need a bigger boat", 30, &nout);

  ASSERT_STRARY_EQ(2, nout, expected, actual);

  wrap_lines_free(actual, nout);
}
END_TEST

START_TEST(test_wrap_lines_with_multiple_spaces) {
  unsigned int nout;
  char *expected[] = {
    "We're going to", "need a bigger", "boat",
  };

  char **actual = wrap_lines("We're going to    need a bigger boat", 14, &nout);

  ASSERT_STRARY_EQ(3, nout, expected, actual);

  wrap_lines_free(actual, nout);
}
END_TEST

START_TEST(test_wrap_lines_does_not_strip_spaces_after_newline) {
  unsigned int nout;
  char *expected[] = {
    "      We're going to need", "      a bigger boat",
  };

  char **actual =
      wrap_lines("      We're going to need\n      a bigger boat", 35, &nout);

  ASSERT_STRARY_EQ(2, nout, expected, actual);

  wrap_lines_free(actual, nout);
}
END_TEST

// probably ideally it would trim a trailing space
// but for the purposes of this, it doesn't really matter
START_TEST(test_wrap_lines_lands_on_a_space) {
  unsigned int nout;
  char *expected[] = {
    "We're going to ", "need a bigger", "boat",
  };

  char **actual = wrap_lines("We're going to    need a bigger boat", 15, &nout);

  ASSERT_STRARY_EQ(3, nout, expected, actual);

  wrap_lines_free(actual, nout);
}
END_TEST

START_TEST(test_wrap_lines_with_no_spaces) {
  unsigned int nout;
  char *expected[] = {
    "You'rego", "ingtonee", "dabigger", "boat",
  };

  char **actual = wrap_lines("You'regoingtoneedabiggerboat", 8, &nout);

  ASSERT_STRARY_EQ(4, nout, expected, actual);

  wrap_lines_free(actual, nout);
}
END_TEST

START_TEST(test_wrap_lines_preserves_newlines) {
  unsigned int nout;
  char *expected[] = {
    "You're", "going", "to need", "a bigger", "boat",
  };

  char **actual =
      wrap_lines("You're\ngoing\nto need\na bigger\nboat", 50, &nout);

  ASSERT_STRARY_EQ(5, nout, expected, actual);

  wrap_lines_free(actual, nout);
}
END_TEST

// We can't accurately test wrapping output because rendering
// depends on where the string is broken, so just check nothing explodes
START_TEST(test_wrap_lines_with_odd_unicode_doesnt_explode) {
  unsigned int nout;

  char **actual =
      wrap_lines("H̕͝ ̛G̢̧̛Ó̧̕D̶͘ ̢ST̶͢ÓP T͠H̷͏E̵̸̛͝ "
                 "P̷͡Ą͠͠͡I̷͟N̶͜!¡!",
                 13, &nout);

  wrap_lines_free(actual, nout);
}
END_TEST

START_TEST(test_wrap_lines_empty_text) {
  unsigned int nout;
  char *expected[] = {
    "",
  };

  char **actual = wrap_lines("", 15, &nout);

  ASSERT_STRARY_EQ(1, nout, expected, actual);

  wrap_lines_free(actual, nout);
}
END_TEST

START_TEST(test_wrap_lines_null_text) {
  unsigned int nout;
  char *expected[] = {
    "",
  };

  char **actual = wrap_lines(NULL, 15, &nout);

  ASSERT_STRARY_EQ(1, nout, expected, actual);

  wrap_lines_free(actual, nout);
}
END_TEST

START_TEST(test_wrap_lines_zero_width) {
  unsigned int nout;
  char *expected[] = {
    "",
  };

  char **actual = wrap_lines("We're going to need a bigger boat", 0, &nout);

  ASSERT_STRARY_EQ(1, nout, expected, actual);

  wrap_lines_free(actual, nout);
}
END_TEST

START_TEST(test_wrap_lines_negative_width) {
  unsigned int nout;
  char *expected[] = {
    "",
  };

  char **actual = wrap_lines("We're going to need a bigger boat", -5, &nout);

  ASSERT_STRARY_EQ(1, nout, expected, actual);

  wrap_lines_free(actual, nout);
}
END_TEST

START_TEST(test_wrap_lines_free) {
  wrap_lines_free(NULL, 0);
}
END_TEST

START_TEST(test_stralleycat) {
  char *strs[] = { "one", "two", "three", "four five" };

  char *result = stralleycat(4, strs);

  ck_assert_str_eq(result, "one two three four five");
  free(result);
}
END_TEST

START_TEST(test_expand_home) {
  char *path = "~/.i_am_a_file";
  setenv("HOME", "/home/bugle", 1);

  char *result = expand_home(path);
  ck_assert_str_eq(result, "/home/bugle/.i_am_a_file");
  free(result);
}
END_TEST

Suite *strutils_suite(void) {
  Suite *s = suite_create("strutils");

  ADD_TEST_CASE(s, test_stralleycat);
  ADD_TEST_CASE(s, test_strcopytrunc);
  ADD_TEST_CASE(s, test_strcopytrunc_width_exceeds_strlen);
  ADD_TEST_CASE(s, test_strcopytrunc_zero);
  ADD_TEST_CASE(s, test_expand_home);
  ADD_TEST_CASE(s, test_count_lines_and_find_length_of_longest);
  ADD_TEST_CASE(s, test_count_lines_and_find_length_of_longest_with_one_line);
  ADD_TEST_CASE(
      s,
      test_count_lines_and_find_length_of_longest_with_one_line_with_trailing_new_line);
  ADD_TEST_CASE(s, test_no_wrap_when_text_fits_on_one_line);
  ADD_TEST_CASE(s, test_lines_wrap_at_13);
  ADD_TEST_CASE(s, test_lines_wrap_at_30);
  ADD_TEST_CASE(s, test_wrap_lines_with_multiple_spaces);
  ADD_TEST_CASE(s, test_wrap_lines_does_not_strip_spaces_after_newline);
  ADD_TEST_CASE(s, test_wrap_lines_lands_on_a_space);
  ADD_TEST_CASE(s, test_wrap_lines_empty_text);
  ADD_TEST_CASE(s, test_wrap_lines_null_text);
  ADD_TEST_CASE(s, test_wrap_lines_free);
  ADD_TEST_CASE(s, test_wrap_lines_with_no_spaces);
  ADD_TEST_CASE(s, test_wrap_lines_preserves_newlines);
  ADD_TEST_CASE(s, test_wrap_lines_with_odd_unicode_doesnt_explode);
  ADD_TEST_CASE(s, test_wrap_lines_zero_width);
  ADD_TEST_CASE(s, test_wrap_lines_negative_width);
  ADD_TEST_CASE(s, test_wrap_lines_free);

  return s;
}

CONCH_CHECK_MAIN(strutils_suite)
