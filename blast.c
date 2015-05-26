#include <stdlib.h>
#include <stdio.h>
#include "backend.h"

int main(int argc, char **argv) {
  int exit_code = 0;
  if (argc == 3) {
    // Connect to postgres
    settings config = {
      .page_size = 42,
    };
    mouthpiece *mp = conch_connect(config);

    blastresult *result = conch_blast_post(mp, argv[1], argv[2], NULL);
    if (result->post == 0) {
      fprintf(stderr, "Fail: %s\n", result->error_message);
      exit_code = 1;
    }
    conch_blastresult_free(result);
    conch_disconnect(mp);
  } else {
    printf("Usage: %s username 'message'\n", argv[0]);
    exit_code = 1;
  }

  return exit_code;
}
