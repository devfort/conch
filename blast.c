#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h>

#include "strutils.h"
#include "config.h"
#include "backend.h"

#define READ_BUFFER_SIZE 1000

typedef struct {
  char *config_filename;
  char *username;
  char *attachment_filename;
  char *extended_filename;
  char *host;
  char *database;
  bool verbose;
  bool extended;
  bool help;
} blast_options;

static void options_empty(blast_options *options) {
  options->config_filename = NULL;
  options->username = NULL;
  options->host = NULL;
  options->database = NULL;
  options->attachment_filename = NULL;
  options->extended_filename = NULL;
  options->verbose = false;
  options->extended = false;
  options->help = false;
}

static void options_defaults(blast_options *options) {
  options_empty(options);
  options->config_filename = DEFAULT_CONFIG_LOCATION;
  options->host = DEFAULT_DATABASE_HOST;
  options->database = DEFAULT_DATABASE_NAME;
}

void blast_merge_options(blast_options *options, settings config,
                         blast_options defaults) {
  if (!options->username) {
    options->username = config.username ? config.username : defaults.username;
  }
  if (!options->host) {
    options->host = config.host ? config.host : defaults.host;
  }
  if (!options->database) {
    options->database = config.database ? config.database : defaults.database;
  }
}

blast_options blast_parse_command_line_args(int argc, char **argv);

void blast_usage(char *arg0) {
  printf("Usage: %s [options] 'message'\n\n", arg0);
  printf("  -u, --username=username\tuser to post as\n"
         "  -c, --config=config\t\tconfig filename (defaults to ~/.conchrc)\n"
         "  -a, --attachment=filename\tname of file to upload and attach to "
         "blast\n"
         "  --extended=filename\t\textended blast from file\n"
         "  -e\t\t\t\textended blast, will wait for input on stdin\n"
         "  -v, --verbose\t\t\tprint out blast details\n");
}

char *get_ext_msg(blast_options options) {
  if (options.extended) {
    FILE *extfile;
    if (options.extended_filename) {
      if (strcmp(options.extended_filename, "-")) {
        extfile = fopen(options.extended_filename, "r");
        if (extfile == NULL) {
          fprintf(stderr, "Couldn't open the file: %s",
                  options.extended_filename);
        }
      } else {
        extfile = stdin;
      }
    } else {
      printf("Enter your extended message\n");
      printf("  press ^D on a new line to end\n");
      extfile = stdin;
    }
    int size = 1;
    int read = 0;
    char *buffer = NULL;
    do {
      size += READ_BUFFER_SIZE;
      buffer = realloc(buffer, size * sizeof(char));
      read += fread(buffer + read, sizeof(char), READ_BUFFER_SIZE, extfile);
    } while (!feof(extfile));
    buffer[read] = '\0';
    return buffer;
  } else {
    return NULL;
  }
}

int main(int argc, char **argv) {
  int exit_code = 0;
  blast_options default_options;
  options_defaults(&default_options);
  blast_options options = blast_parse_command_line_args(argc, argv);
  settings config = conch_load_config(options.config_filename);
  blast_merge_options(&options, config, default_options);

  if (options.username == NULL) {
    options.help = true;
  }

  if (options.help || argv[optind] == NULL) {
    blast_usage(argv[0]);
    exit_code = 1;
  } else {
    char *msg = stralleycat(argc - optind, argv + optind);
    mouthpiece *mp = conch_connect(config);

    char *ext_msg = get_ext_msg(options);

    blastresult *result = conch_blast_post(mp, options.username, msg, ext_msg);
    if (result->post == 0) {
      fprintf(stderr, "Fail: %s\n", result->error_message);
      exit_code = 1;
    }
    if (options.verbose) {
      printf("posting as: %s\n", options.username);
      printf("posting to: %s\n", config.host);
      printf("blast id: %" PRIid "\n", result->post);
    }
    conch_blastresult_free(result);
    conch_disconnect(mp);
    free(msg);
    free(ext_msg);
  }

  return exit_code;
}

blast_options blast_parse_command_line_args(int argc, char **argv) {
  blast_options parsed_options = {.config_filename = DEFAULT_CONFIG_LOCATION,
                                  .host = DEFAULT_DATABASE_HOST,
                                  .database = DEFAULT_DATABASE_NAME,
                                  .extended = false,
                                  .help = false };

  int opt;
  static struct option longopts[] = {
    { "config", required_argument, NULL, 'c' },
    { "user", required_argument, NULL, 'u' },
    { "attachment", required_argument, NULL, 'a' },
    { "extended", optional_argument, NULL, 'e' },
    { "help", no_argument, NULL, 'h' },
    { NULL, 0, NULL, 0 },
  };

  while ((opt = getopt_long(argc, argv, "h?c:u:a:ev", longopts, NULL)) != -1) {
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
  return parsed_options;
}
