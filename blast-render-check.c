#include <stdio.h>
#include "checkrunner.h"

#include "blastlist.h"
#include "blast-render.h"

// This is defined inside the blast-render.c but is needed nowhere else in the
// codebase except this test so rather than have it in a header file lets
// redefine it here
#define BLAST_EXTENDED_MARKER "[...]"

START_TEST(test_conch_blast_prepare_null) {
  drawlist *instructions;

  instructions = conch_blast_prepare(NULL, 10, true);

  ASSERT_PTR_NULL(instructions);

  conch_drawlist_free(instructions);
}
END_TEST

START_TEST(test_conch_blast_prepare_oneline) {
  drawlist *instructions;
  blast b = {
    .user = "joebloggs",
    .content = "This line is 27 chars long.",
    .posted_at = "two blue moons ago",
  };

  instructions = conch_blast_prepare(&b, 40, true);

  ASSERT_PTR_NOT_NULL(instructions);
  ck_assert(!instructions->has_marker);
  ck_assert_int_eq(instructions->nlines, 2);
  ck_assert_str_eq(instructions->content[0], b.content);
  ASSERT_PTR_NOT_NULL(strstr(instructions->content[1], b.user));
  ASSERT_PTR_NOT_NULL(strstr(instructions->content[1], b.posted_at));

  conch_drawlist_free(instructions);
}
END_TEST

START_TEST(test_conch_blast_prepare_multiline) {
  drawlist *instructions;
  blast b = {
    .user = "joe",
    .content = "This line is 27 chars long.",
    .posted_at = "2:22",
  };

  instructions = conch_blast_prepare(&b, 15, true);

  ASSERT_PTR_NOT_NULL(instructions);
  ck_assert(!instructions->has_marker);
  ck_assert_int_eq(instructions->nlines, 3);
  ASSERT_PTR_NOT_NULL(strstr(instructions->content[2], b.user));
  ASSERT_PTR_NOT_NULL(strstr(instructions->content[2], b.posted_at));

  conch_drawlist_free(instructions);
}
END_TEST

START_TEST(test_conch_blast_prepare_attribution_line_truncated) {
  drawlist *instructions;
  blast b = {
    .user = "joanna",
    .content = "This line is 27 chars long.",
    .posted_at = "two blue moons ago",
  };

  instructions = conch_blast_prepare(&b, 15, true);

  ASSERT_PTR_NOT_NULL(instructions);
  ck_assert(!instructions->has_marker);
  ck_assert_int_eq(instructions->nlines, 3);
  ASSERT_PTR_NOT_NULL(strstr(instructions->content[2], b.user));
  ASSERT_PTR_NULL(strstr(instructions->content[2], b.posted_at));
  ASSERT_PTR_NOT_NULL(strstr(instructions->content[2], "tw"));

  conch_drawlist_free(instructions);
}
END_TEST

START_TEST(test_conch_blast_prepare_attachment) {
  drawlist *instructions;
  blast b = {
    .user = "joebloggs",
    .content = "This line is 27 chars long.",
    .posted_at = "two blue moons ago",
    .attachment = "http://example.com/foo.zip",
  };

  instructions = conch_blast_prepare(&b, 40, true);

  ASSERT_PTR_NOT_NULL(instructions);
  ck_assert(!instructions->has_marker);
  ck_assert_int_eq(instructions->nlines, 3);
  ck_assert_str_eq(instructions->content[1], b.attachment);

  conch_drawlist_free(instructions);
}
END_TEST

START_TEST(test_conch_blast_prepare_extended_with_display_markers) {
  drawlist *instructions;
  blast b = {
    .user = "joebloggs",
    .content = "This line is 27 chars long.",
    .posted_at = "two blue moons ago",
    .extended = "LOOK AT MY EXTENSION!",
  };

  bool display_markers = true;

  instructions = conch_blast_prepare(&b, 40, display_markers);

  ASSERT_PTR_NOT_NULL(instructions);
  ck_assert(instructions->has_marker);
  ck_assert_int_eq(instructions->nlines, 2);
  ASSERT_PTR_NULL(strstr(instructions->content[0], BLAST_EXTENDED_MARKER));
  ck_assert_int_eq(instructions->content_last_line, 0);
  ck_assert_int_eq(instructions->last_line_length, 28);

  conch_drawlist_free(instructions);
}
END_TEST

START_TEST(test_conch_blast_prepare_extended_without_display_markers) {
  drawlist *instructions;
  blast b = {
    .user = "joebloggs",
    .content = "This line is 27 chars long.",
    .posted_at = "two blue moons ago",
    .extended = "LOOK AT MY EXTENSION!",
  };

  bool display_markers = false;

  instructions = conch_blast_prepare(&b, 40, display_markers);

  ASSERT_PTR_NOT_NULL(instructions);
  ck_assert(!instructions->has_marker);
  ck_assert_int_eq(instructions->nlines, 2);
  ASSERT_PTR_NULL(strstr(instructions->content[0], BLAST_EXTENDED_MARKER));
  ck_assert_int_eq(instructions->content_last_line, 0);
  ck_assert_int_eq(instructions->last_line_length, 0);

  conch_drawlist_free(instructions);
}
END_TEST

START_TEST(test_conch_blast_prepare_extended_marker_at_start_of_line) {
  drawlist *instructions;
  blast b = {
    .user = "joebloggs",
    .content = "areallylongstringwithnospaces",
    .posted_at = "two blue moons ago",
    .extended = "LOOK AT MY EXTENSION!",
  };

  instructions = conch_blast_prepare(&b, 29, true);

  ASSERT_PTR_NOT_NULL(instructions);
  ck_assert(instructions->has_marker);
  ck_assert_int_eq(instructions->nlines, 3);
  ASSERT_PTR_NULL(strstr(instructions->content[0], BLAST_EXTENDED_MARKER));
  // This line is empty because the marker has wrapped to a new line
  // but does not display.
  ck_assert_str_eq(instructions->content[1], "");
  ck_assert_int_eq(instructions->content_last_line, 1);
  ck_assert_int_eq(instructions->last_line_length, 0);

  conch_drawlist_free(instructions);
}
END_TEST

START_TEST(test_conch_blast_extended_in_wrong_place_with_unicode) {
  drawlist *instructions;
  blast b = {
    .user = "Unicode user",
    .content = "123…",
    .posted_at = "two blue moons ago",
    .extended = "LOOK AT MY EXTENSION!",
  };

  instructions = conch_blast_prepare(&b, 20, true);

  ASSERT_PTR_NOT_NULL(instructions);
  ASSERT_PTR_NULL(strstr(instructions->content[0], BLAST_EXTENDED_MARKER));
  ck_assert_int_eq(instructions->content_last_line, 0);
  // our strlen counts the ellipsis as 3 characters (+ terminating char = 7)
  // but it displays as one.
  ck_assert_int_eq(instructions->last_line_length, 7);

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
  ADD_TEST_CASE(s, test_conch_blast_prepare_attribution_line_truncated);
  ADD_TEST_CASE(s, test_conch_blast_extended_in_wrong_place_with_unicode);

  return s;
}

CONCH_CHECK_MAIN(blast_render_suite)
