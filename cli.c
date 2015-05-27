#include <getopt.h>
#include <stdbool.h>
#include <stdlib.h>

#include "cli.h"

conch_cli_options conch_parse_command_line_args(int argc, char **argv) {
  conch_cli_options parsed_options = {
    .stick_to_top = false, .host = "bugle.fort",
  };
  int opt;
  static struct option longopts[] = {
    { "stick-to-top", no_argument, NULL, 's' },
    { "host", required_argument, NULL, 'h' },
    { "database", required_argument, NULL, 'd' },
    { NULL, 0, NULL, 0 },
  };

  while ((opt = getopt_long(argc, argv, "sh:d:", longopts, NULL)) != -1) {
    switch (opt) {
    case 's':
      parsed_options.stick_to_top = true;
      break;
    case 'h':
      parsed_options.host = optarg;
      break;
    case 'd':
      parsed_options.database = optarg;
      break;
    }
  }
  argc -= optind;
  argv += optind;

  return parsed_options;
}
