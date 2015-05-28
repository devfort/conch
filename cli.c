#include <getopt.h>
#include <stdbool.h>
#include <stdlib.h>

#include "config.h"
#include "cli.h"
#include "backend.h"

static void merge_config(conch_cli_options *options, settings *config) {
  if (!options->host) {
    options->host = config->host;
  }
  if (!options->database) {
    options->database = config->database;
  }
}

conch_cli_options conch_parse_command_line_args(int argc, char **argv) {
  conch_cli_options parsed_options = {
    .stick_to_top = false, .host = "bugle.fort",
  };
  int opt;
  char *config_filename = DEFAULT_CONFIG_LOCATION;
  static struct option longopts[] = {
    { "stick-to-top", no_argument, NULL, 's' },
    { "config", required_argument, NULL, 'c' },
    { "host", required_argument, NULL, 'h' },
    { "database", required_argument, NULL, 'd' },
    { NULL, 0, NULL, 0 },
  };

  while ((opt = getopt_long(argc, argv, "sc:h:d:", longopts, NULL)) != -1) {
    switch (opt) {
    case 'c':
      config_filename = optarg;
      break;
    case 's':
      parsed_options.stick_to_top = true;
      break;
    case 'h':
      parsed_options.host = optarg;
      break;
    case 'd':
      parsed_options.database = optarg;
      break;
    default:
      exit(1);
    }
  }

  settings config = conch_load_config(config_filename);
  merge_config(&parsed_options, &config);

  return parsed_options;
}
