#include <stdio.h>
#include "checkrunner.h"

#include "blastlist.h"
#include "blast-render.h"

// This is defined inside the blast-render.c but is needed nowhere else in the
// codebase except this test so rather than have it in a header file lets
// redefine it here
#define BLAST_EXTENDED_MARKER "[...]"

START_TEST(test_conch_blast_prepare_null) {
  int nlines;
  drawlist *instructions;

  instructions = conch_blast_prepare(NULL, 10, &nlines, true);

  ASSERT_PTR_NULL(instructions);
  ck_assert_int_eq(nlines, 0);

  conch_drawlist_free(instructions);
}
END_TEST

START_TEST(test_conch_blast_prepare_oneline) {
  int nlines;
  drawlist *instructions;
  blast b = {
    .user = "joebloggs",
    .content = "This line is 27 chars long.",
    .posted_at = "two blue moons ago",
  };

  instructions = conch_blast_prepare(&b, 40, &nlines, true);

  ASSERT_PTR_NOT_NULL(instructions);
  ck_assert(!instructions->has_marker);
  ck_assert_int_eq(nlines, 2);
  ck_assert_str_eq(instructions->content[0], b.content);
  ASSERT_PTR_NOT_NULL(strstr(instructions->content[1], b.user));
  ASSERT_PTR_NOT_NULL(strstr(instructions->content[1], b.posted_at));
  ASSERT_PTR_NULL(instructions->content[2]);

  conch_drawlist_free(instructions);
}
END_TEST

START_TEST(test_conch_blast_prepare_multiline) {
  int nlines;
  drawlist *instructions;
  blast b = {
    .user = "joebloggs",
    .content = "This line is 27 chars long.",
    .posted_at = "two blue moons ago",
  };

  instructions = conch_blast_prepare(&b, 15, &nlines, true);

  ASSERT_PTR_NOT_NULL(instructions);
  ck_assert(!instructions->has_marker);
  ck_assert_int_eq(nlines, 3);
  ASSERT_PTR_NOT_NULL(strstr(instructions->content[2], b.user));
  ASSERT_PTR_NOT_NULL(strstr(instructions->content[2], b.posted_at));
  ASSERT_PTR_NULL(instructions->content[3]);

  conch_drawlist_free(instructions);
}
END_TEST

START_TEST(test_conch_blast_prepare_attachment) {
  int nlines;
  drawlist *instructions;
  blast b = {
    .user = "joebloggs",
    .content = "This line is 27 chars long.",
    .posted_at = "two blue moons ago",
    .attachment = "http://example.com/foo.zip",
  };

  instructions = conch_blast_prepare(&b, 40, &nlines, true);

  ASSERT_PTR_NOT_NULL(instructions);
  ck_assert(!instructions->has_marker);
  ck_assert_int_eq(nlines, 3);
  ck_assert_str_eq(instructions->content[1], b.attachment);
  ASSERT_PTR_NULL(instructions->content[3]);

  conch_drawlist_free(instructions);
}
END_TEST

START_TEST(test_conch_blast_prepare_extended_with_display_markers) {
  int nlines;
  drawlist *instructions;
  blast b = {
    .user = "joebloggs",
    .content = "This line is 27 chars long.",
    .posted_at = "two blue moons ago",
    .extended = "LOOK AT MY EXTENSION!",
  };

  bool display_markers = true;

  instructions = conch_blast_prepare(&b, 40, &nlines, display_markers);

  ASSERT_PTR_NOT_NULL(instructions);
  ck_assert(instructions->has_marker);
  ck_assert_int_eq(nlines, 2);
  ASSERT_PTR_NULL(strstr(instructions->content[0], BLAST_EXTENDED_MARKER));
  ck_assert_int_eq(instructions->marker_rel_y, 0);
  ck_assert_int_eq(instructions->marker_rel_x, 28);
  ASSERT_PTR_NULL(instructions->content[2]);

  conch_drawlist_free(instructions);
}
END_TEST

START_TEST(test_conch_blast_prepare_extended_without_display_markers) {
  int nlines;
  drawlist *instructions;
  blast b = {
    .user = "joebloggs",
    .content = "This line is 27 chars long.",
    .posted_at = "two blue moons ago",
    .extended = "LOOK AT MY EXTENSION!",
  };

  bool display_markers = false;

  instructions = conch_blast_prepare(&b, 40, &nlines, display_markers);

  ASSERT_PTR_NOT_NULL(instructions);
  ck_assert(!instructions->has_marker);
  ck_assert_int_eq(nlines, 2);
  ASSERT_PTR_NULL(strstr(instructions->content[0], BLAST_EXTENDED_MARKER));
  ck_assert_int_eq(instructions->marker_rel_y, 0);
  ck_assert_int_eq(instructions->marker_rel_x, 0);
  ASSERT_PTR_NULL(instructions->content[2]);

  conch_drawlist_free(instructions);
}
END_TEST

START_TEST(test_conch_blast_prepare_extended_marker_at_start_of_line) {
  int nlines;
  drawlist *instructions;
  blast b = {
    .user = "joebloggs",
    .content = "areallylongstringwithnospaces",
    .posted_at = "two blue moons ago",
    .extended = "LOOK AT MY EXTENSION!",
  };

  instructions = conch_blast_prepare(&b, 29, &nlines, true);

  ASSERT_PTR_NOT_NULL(instructions);
  ck_assert(instructions->has_marker);
  ck_assert_int_eq(nlines, 3);
  ASSERT_PTR_NULL(strstr(instructions->content[0], BLAST_EXTENDED_MARKER));
  // Becuase "[...]" should have wrapped to a new line, and it shouldn't appear
  // in the content to print the second line should be empty
  ck_assert_str_eq(instructions->content[1], "");
  ck_assert_int_eq(instructions->marker_rel_y, 1);
  ck_assert_int_eq(instructions->marker_rel_x, 0);
  ASSERT_PTR_NULL(instructions->content[3]);

  conch_drawlist_free(instructions);
}
END_TEST

Suite *blast_render_suite(void) {
  Suite *s = suite_create("blast_render");

  ADD_TEST_CASE(s, test_conch_blast_prepare_null);
  ADD_TEST_CASE(s, test_conch_blast_prepare_oneline);
  ADD_TEST_CASE(s, test_conch_blast_prepare_multiline);
  ADD_TEST_CASE(s, test_conch_blast_prepare_attachment);
  ADD_TEST_CASE(s, test_conch_blast_prepare_extended_with_display_markers);
  ADD_TEST_CASE(s, test_conch_blast_prepare_extended_without_display_markers);
  ADD_TEST_CASE(s, test_conch_blast_prepare_extended_marker_at_start_of_line);

  return s;
}

CONCH_CHECK_MAIN(blast_render_suite)
