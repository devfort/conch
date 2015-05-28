#ifndef _CONFIG_H
#define _CONFIG_H

#include "backend.h"

#define DEFAULT_PAGE_SIZE 42
#define DEFAULT_CONFIG_LOCATION "~/.conchrc"

settings conch_load_config(char *filename);

#endif
