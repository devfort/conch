#ifndef _CLI_H
#define _CLI_H

#include <stdbool.h>

typedef struct { bool stick_to_top; } conch_cli_options;

conch_cli_options conch_parse_command_line_args(int argc, char **argv);

#endif /* _CLI_H */
