#include <getopt.h>
#include <stdbool.h>

#include "cli.h"

conch_cli_options conch_parse_command_line_args(int argc, char **argv) {
  conch_cli_options parsed_options = {
    .stick_to_top = false,
  };
  int opt;
  static struct option longopts[] = {
    { "stick-to-top", no_argument, NULL, 's' }, { NULL, 0, NULL, 0 },
  };

  while ((opt = getopt_long(argc, argv, "s", longopts, NULL)) != -1) {
    switch (opt) {
    case 's':
      parsed_options.stick_to_top = true;
      break;
    }
  }
  argc -= optind;
  argv += optind;

  return parsed_options;
}
