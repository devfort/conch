#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h>

#include "strutils.h"
#include "backend.h"

typedef struct {
  char *config_filename;
  char *username;
  char *attachment_filename;
  char *extended_filename;
  bool verbose;
  bool extended;
  bool help;
} blast_options;

blast_options blast_parse_command_line_args(int argc, char **argv);

void blast_usage(char *arg0) {
  printf("Usage: %s [options] 'message'\n", arg0);
  printf("  -u username\n"
    "  --username=username\n"
    "  -c config\t\t\tconfig filename (defaults to ~/.conchrc)\n"
    "  --config=config\n"
    "  -a filename\t\t\tname of file to upload and attach to blast\n"
    "  --attachment=filename\n"
    "  -e\t\t\t\textended blast, will wait for input on stdin\n"
    "  --extended=filename\t\twill read extended blast content from filename\n"
    "  -v\n"
    "  --verbose\t\t\tprint out blast details\n");
}

int main(int argc, char **argv) {
  int exit_code = 0;
  blast_options options = blast_parse_command_line_args(argc, argv);

  if (options.help || argv[optind] == NULL) {
    blast_usage(argv[0]);
    exit_code = 1;
  } else {
    char *msg = stralleycat(argc - optind, argv + optind);
    settings config = {
      .page_size = 1,
    };
    mouthpiece *mp = conch_connect(config);

    blastresult *result = conch_blast_post(mp, options.username, msg, NULL);
    if (result->post == 0) {
      fprintf(stderr, "Fail: %s\n", result->error_message);
      exit_code = 1;
    }
    conch_blastresult_free(result);
    conch_disconnect(mp);
    free(msg);
  }

  return exit_code;
}

blast_options blast_parse_command_line_args(int argc, char **argv) {
  blast_options parsed_options = {
    .config_filename = "~/.conchrc",
    .extended = false,
    .help = false
  };

  int opt;
  static struct option longopts[] = {
    { "config", required_argument, NULL, 'c'},
    { "user", required_argument, NULL, 'u' },
    { "attachment", required_argument, NULL, 'a' },
    { "extended", optional_argument, NULL, 'e' },
    { "help", no_argument, NULL, 'h' },
    { NULL, 0, NULL, 0 },
  };

  while ((opt = getopt_long(argc, argv, "c:u:a:ev", longopts, NULL)) != -1) {
    switch (opt) {
    case 'c':
      parsed_options.config_filename = optarg;
      break;
    case 'u':
      parsed_options.username = optarg;
      break;
    case 'a':
      parsed_options.attachment_filename = optarg;
      break;
    case 'e':
      parsed_options.extended = true;
      if (optarg) {
        parsed_options.extended_filename = optarg;
      }
      break;
    case 'v':
      parsed_options.verbose = true;
      break;
    case 'h':
    case '?':
    default:
      parsed_options.help = true;
    }
  }

  if (parsed_options.username == NULL) {
    parsed_options.help = true;
  }
  return parsed_options;
}

