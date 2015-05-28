#include <stdio.h>
#include "checkrunner.h"

#include "blastlist.h"
#include "blast-render.h"

START_TEST(test_conch_blast_prepare_null) {
  int nlines;
  char **lines;

  lines = conch_blast_prepare(NULL, 10, &nlines);

  ASSERT_PTR_NULL(lines);
  ck_assert_int_eq(nlines, 0);
}
END_TEST

START_TEST(test_conch_blast_prepare_oneline) {
  int nlines;
  char **lines;
  blast b = {
    .user = "joebloggs",
    .content = "This line is 27 chars long.",
    .posted_at = "two blue moons ago",
  };

  lines = conch_blast_prepare(&b, 40, &nlines);

  ASSERT_PTR_NOT_NULL(lines);
  ck_assert_int_eq(nlines, 2);
  ck_assert_str_eq(lines[0], b.content);
  ASSERT_PTR_NOT_NULL(strstr(lines[1], b.user));
  ASSERT_PTR_NOT_NULL(strstr(lines[1], b.posted_at));
  ASSERT_PTR_NULL(lines[2]);
}
END_TEST

START_TEST(test_conch_blast_prepare_multiline) {
  int nlines;
  char **lines;
  blast b = {
    .user = "joebloggs",
    .content = "This line is 27 chars long.",
    .posted_at = "two blue moons ago",
  };

  lines = conch_blast_prepare(&b, 15, &nlines);

  ASSERT_PTR_NOT_NULL(lines);
  ck_assert_int_eq(nlines, 3);
  ASSERT_PTR_NOT_NULL(strstr(lines[2], b.user));
  ASSERT_PTR_NOT_NULL(strstr(lines[2], b.posted_at));
  ASSERT_PTR_NULL(lines[3]);
}
END_TEST

START_TEST(test_conch_blast_prepare_attachment) {
  int nlines;
  char **lines;
  blast b = {
    .user = "joebloggs",
    .content = "This line is 27 chars long.",
    .posted_at = "two blue moons ago",
    .attachment = "http://example.com/secretstashofimagesofsomedescription.zip",
  };

  lines = conch_blast_prepare(&b, 40, &nlines);

  ASSERT_PTR_NOT_NULL(lines);
  ck_assert_int_eq(nlines, 3);
  ck_assert_str_eq(lines[1], b.attachment);
  ASSERT_PTR_NULL(lines[3]);
}
END_TEST

START_TEST(test_conch_blast_prepare_extended) {
  int nlines;
  char **lines;
  blast b = {
    .user = "joebloggs",
    .content = "This line is 27 chars long.",
    .posted_at = "two blue moons ago",
    .extended = "LOOK AT MY EXTENSION!",
  };

  lines = conch_blast_prepare(&b, 40, &nlines);

  ASSERT_PTR_NOT_NULL(lines);
  ck_assert_int_eq(nlines, 2);
  ASSERT_PTR_NOT_NULL(strstr(lines[1], BLAST_EXTENDED_MARKER));
  ASSERT_PTR_NULL(lines[2]);
}
END_TEST

Suite *blast_render_suite(void) {
  Suite *s = suite_create("blast_render");

  ADD_TEST_CASE(s, test_conch_blast_prepare_null);
  ADD_TEST_CASE(s, test_conch_blast_prepare_oneline);
  ADD_TEST_CASE(s, test_conch_blast_prepare_multiline);
  ADD_TEST_CASE(s, test_conch_blast_prepare_attachment);
  ADD_TEST_CASE(s, test_conch_blast_prepare_extended);

  return s;
}

CONCH_CHECK_MAIN(blast_render_suite)
