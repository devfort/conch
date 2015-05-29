#ifndef _CONFIG_H
#define _CONFIG_H

#include "backend.h"

#define DEFAULT_PAGE_SIZE 42
#define DEFAULT_CONFIG_LOCATION "~/.conchrc"

void generate_clock_text(int time_str_limit, char *time_str);
settings conch_load_config(const char *filename);

#endif
