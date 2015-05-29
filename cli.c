#include <getopt.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

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

void usage(char const *const progname) {
  printf("Usage: %s [OPTIONS]\n", progname);
  printf("ncurses client for bugle.\n");

  printf("\nGeneral options:\n");
  printf("  -h, --help\t\tdisplay this help and exit\n");
  printf("  -s, --stick-to-top\tauto-display new blasts\n");
  printf("  -c, --config=FILE\tuse FILE for configuration (default: %s)\n",
         DEFAULT_CONFIG_LOCATION);
  printf("\nOptions (these override the config file):\n");
  printf("  -u, --username=USER\tuser to post as (register via bugle)\n");
  printf("  -H, --host=HOSTNAME\tdatabase server host (default: %s)\n",
         DEFAULT_DATABASE_HOST);
  printf("  -d, --database=DBNAME\tdatabase name (default: %s)\n",
         DEFAULT_DATABASE_NAME);
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
    { "help", no_argument, NULL, 'h' },
    { "stick-to-top", no_argument, NULL, 's' },
    { "config", required_argument, NULL, 'c' },
    { "host", required_argument, NULL, 'H' },
    { "database", required_argument, NULL, 'd' },
    { "username", required_argument, NULL, 'u' },
    { NULL, 0, NULL, 0 },
  };

  while ((opt = getopt_long(argc, argv, "h?sc:H:d:u:", longopts, NULL)) != -1) {
    switch (opt) {
    case 'h':
    case '?':
      usage(argv[0]);
      exit(0);
    case 'c':
      config_filename = optarg;
      break;
    case 's':
      parsed_options.stick_to_head = true;
      break;
    case 'H':
      parsed_options.host = optarg;
      break;
    case 'd':
      parsed_options.database = optarg;
      break;
    case 'u':
      parsed_options.username = optarg;
      break;
    default:
      usage(argv[0]);
      exit(1);
    }
  }

  settings config = conch_load_config(config_filename);
  merge_config(&parsed_options, &config);

  return parsed_options;
}
