#include <curses.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "anigif.h"
#include "detailview.h"
#include "strutils.h"

#define FALLBACK_EDITOR "vi"

detailview *conch_detailview_new(blastlist *blastlist) {
  detailview *v = calloc(1, sizeof(detailview));
  v->blastlist = blastlist;

  if (blastlist->current->attachment) {
    v->attachment = conch_webfetcher_get(blastlist->current->attachment);

    // If we have an attachment and it's Content-Type from the server is
    // image/* then try to display it
    if (v->attachment &&
        strncmp(v->attachment->content_type, "image/", strlen("image/")) == 0) {
      v->anigif = anigif_new_from_blob(v->attachment->content,
                                       v->attachment->content_length);
    }
  }
  return v;
}

void conch_detailview_free(detailview *v) {
  if (v) {
    conch_webfetch_result_free(v->attachment);
    anigif_free(v->anigif);
  }
  free(v);
}

void conch_detailview_scroll_down(detailview *v) {
  // detailview_render will clamp this for us.
  v->line_offset++;
}

void conch_detailview_scroll_up(detailview *v) {
  if (v->line_offset > 0) {
    v->line_offset--;
  }
}

void conch_detailview_scroll_code_left(detailview *v) {
  if (v->code_column_offset > 0) {
    v->code_column_offset--;
  }
}

void conch_detailview_scroll_code_right(detailview *v) {
  // detailview_render will clamp this for us.
  v->code_column_offset++;
}

static const char *get_editor() {
  char *editor;

  editor = getenv("VISUAL");
  if (editor) {
    return editor;
  }
  editor = getenv("EDITOR");
  if (editor) {
    return editor;
  }

  return FALLBACK_EDITOR;
}

static char *tmp_blast_file_create(blast *b) {
  char *template = NULL, *filename;
  FILE *file = NULL;

  char *tmpdir = getenv("TMPDIR");
#ifdef P_tmpdir
  if (!tmpdir || strlen(tmpdir) == 0) {
    tmpdir = P_tmpdir;
  } else if (!tmpdir) {
#else
  if (!tmpdir || strlen(tempdir) == 0) {
#endif
    tmpdir = "/tmp";
  }

  // Try 10 times to find a non-conflicting filename.
  for (int i = 0; file == NULL && i < 10; i++) {
    // Free from last time around the loop if we had one.
    free(template);
    template = strcopycat(tmpdir, "/conch-blast.XXXXXX");

    // This modifies template in place, and returns it (or on error returns
    // null)
    filename = mktemp(template);

    if (!filename || filename[0] == '\0') {
      continue;
    }

    // "wx" says create, but only if it didn't already eixst. If it existed
    // return an error. This is how we make sure we don't overwrite someone
    // else's file
    file = fopen(filename, "wx");
  }

  // Still couldn't get a file open? :(
  if (!file) {
    perror("Could not get a tempfile after 10 tries");
    free(template);
    abort();
  }

  const size_t len = strlen(b->extended);
  if (fwrite(b->extended, 1, len, file) != len) {
    errno = ferror(file);
    perror("Could not write extended blast into tmpfile");
    free(template);
    abort();
  }

  fclose(file);

  return filename;
}

static void tmp_blast_file_cleanup(const char *filename) {
  unlink(filename);
  free((void *)filename);
}

static void invoke_editor(const char *const editor,
                          const char *const filename) {
  pid_t fork_pid = fork();

  if (fork_pid == -1) {
    // At this point we are already out of ncurses mode
    perror("Could not invoke editor");
    abort();
  } else if (fork_pid == 0) {
    // Child process
    char *const args[] = { (char *const)editor, (char *const)filename, NULL };

    execvp(editor, args);
    // Should almost never happen
    perror("Error invoking editor");
    abort();
  } else {
    // Parent process
    int stat;
    pid_t signaled_pid = waitpid(fork_pid, &stat, 0);
    int save = errno;
    // Clean up on error or success
    tmp_blast_file_cleanup(filename);

    // Then see if it worked.
    if (signaled_pid == -1) {
      errno = save;
      perror("Error waiting for editor process to finish");
      abort();
    }
  }
}

void conch_detailview_edit_code_snippet(detailview *v) {
  if (!v->blastlist->current->extended)
    return;

  // Save the snippet to a temporary file, invoke editor (VISUAL, EDITOR then
  // `vi`) and delete the temp file afterwards

  // Save our ncurses setup before going back to console mode
  def_prog_mode();
  endwin();

  const char *const editor = get_editor();
  const char *const filename = tmp_blast_file_create(v->blastlist->current);

  invoke_editor(editor, filename);

  reset_prog_mode();
  refresh();
}
