#include <getopt.h>
#include <stdbool.h>
#include <stdlib.h>

#include "config.h"
#include "cli.h"
#include "backend.h"

static void merge_config(conch_cli_options *options, settings *config) {
  if (!options->username) {
    options->username = config->username;
  }
  if (!options->host) {
    options->host = config->host;
  }
  if (!options->database) {
    options->database = config->database;
  }
}

conch_cli_options conch_parse_command_line_args(int argc, char **argv) {
  conch_cli_options parsed_options = {
    .stick_to_head = false,
    .host = DEFAULT_DATABASE_HOST,
    .database = DEFAULT_DATABASE_NAME,
  };
  int opt;
  char *config_filename = DEFAULT_CONFIG_LOCATION;
  static struct option longopts[] = {
    { "stick-to-top", no_argument, NULL, 's' },
    { "config", required_argument, NULL, 'c' },
    // TODO: -h should possibly be equivalent to --help (also make that)
    { "host", required_argument, NULL, 'h' },
    { "database", required_argument, NULL, 'd' },
    { "username", required_argument, NULL, 'u' },
    { NULL, 0, NULL, 0 },
  };

  while ((opt = getopt_long(argc, argv, "sc:h:d:u:", longopts, NULL)) != -1) {
    switch (opt) {
    case 'c':
      config_filename = optarg;
      break;
    case 's':
      parsed_options.stick_to_head = true;
      break;
    case 'h':
      parsed_options.host = optarg;
      break;
    case 'd':
      parsed_options.database = optarg;
      break;
    case 'u':
      parsed_options.username = optarg;
      break;
    default:
      exit(1);
    }
  }

  settings config = conch_load_config(config_filename);
  merge_config(&parsed_options, &config);

  return parsed_options;
}
