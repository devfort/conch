#include "checkrunner.h"

#include "wrap-blast-lines.h"

START_TEST(test_blast_fits_on_one_line) {
  char *expected_blast_lines[1024];
  char **actual_blast_lines;

  expected_blast_lines[0] = "We're going to need a bigger boat";
  expected_blast_lines[1] = NULL;

  actual_blast_lines =
      conch_blast_lines("We're going to need a bigger boat", 42);
  for (int i = 0; expected_blast_lines[i]; i++) {
    ck_assert_str_eq(expected_blast_lines[i], actual_blast_lines[i]);
  }
}
END_TEST

START_TEST(test_blast_lines_13) {
  char *expected_blast_lines[1024];
  char **actual_blast_lines;

  expected_blast_lines[0] = "We're going";
  expected_blast_lines[1] = "to need a";
  expected_blast_lines[2] = "bigger boat";
  expected_blast_lines[3] = NULL;

  actual_blast_lines =
      conch_blast_lines("We're going to need a bigger boat", 13);
  for (int i = 0; expected_blast_lines[i]; i++) {
    ck_assert_str_eq(expected_blast_lines[i], actual_blast_lines[i]);
  }
}
END_TEST

START_TEST(test_blast_lines_30) {
  char *expected_blast_lines[1024];
  char **actual_blast_lines;

  expected_blast_lines[0] = "We're going to need a bigger";
  expected_blast_lines[1] = "boat";
  expected_blast_lines[2] = NULL;

  actual_blast_lines =
      conch_blast_lines("We're going to need a bigger boat", 30);
  for (int i = 0; expected_blast_lines[i]; i++) {
    ck_assert_str_eq(expected_blast_lines[i], actual_blast_lines[i]);
  }
}
END_TEST

START_TEST(test_blast_lines_multiple_spaces) {
  char *expected_blast_lines[1024];
  char **actual_blast_lines;

  expected_blast_lines[0] = "We're going to";
  expected_blast_lines[1] = "need a bigger";
  expected_blast_lines[2] = "boat";
  expected_blast_lines[3] = NULL;

  actual_blast_lines =
      conch_blast_lines("We're going to    need a bigger boat", 14);
  for (int i = 0; expected_blast_lines[i]; i++) {
    ck_assert_str_eq(expected_blast_lines[i], actual_blast_lines[i]);
  }
}
END_TEST

// probably ideally it would trim a trailing space
// but for the purposes of this, it doesn't really matter
START_TEST(test_blast_lines_lands_on_a_space) {
  char *expected_blast_lines[1024];
  char **actual_blast_lines;

  expected_blast_lines[0] = "We're going to "; // note the trailing space
  expected_blast_lines[1] = "need a bigger";
  expected_blast_lines[2] = "boat";
  expected_blast_lines[3] = NULL;

  actual_blast_lines =
      conch_blast_lines("We're going to    need a bigger boat", 15);
  for (int i = 0; expected_blast_lines[i]; i++) {
    ck_assert_str_eq(expected_blast_lines[i], actual_blast_lines[i]);
  }

}
END_TEST

// START_TEST(test_blast_lines_empty_blast) {
//   char *expected_blast_lines[1024];
//   char **actual_blast_lines;
//
//   expected_blast_lines[0] = "";
//   expected_blast_lines[1] = NULL;
//
//   actual_blast_lines = conch_blast_lines("", 15);
//   for (int i = 0; expected_blast_lines[i]; i++) {
//     ck_assert_str_eq(expected_blast_lines[i], actual_blast_lines[i]);
//   }
// }
// END_TEST

Suite *blast_lines_suite(void) {
  Suite *s = suite_create("blast_lines");

  ADD_TEST_CASE(s, test_blast_fits_on_one_line);
  ADD_TEST_CASE(s, test_blast_lines_13);
  ADD_TEST_CASE(s, test_blast_lines_30);
  ADD_TEST_CASE(s, test_blast_lines_multiple_spaces);
  ADD_TEST_CASE(s, test_blast_lines_lands_on_a_space);
  // ADD_TEST_CASE(s, test_blast_lines_empty_blast);

  return s;
}

CONCH_CHECK_MAIN(blast_lines_suite)
