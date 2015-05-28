#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "config.h"
#include "strutils.h"

void parse_config(const char *filename, settings *settings) {
  lua_State *L = luaL_newstate();
  if(luaL_dofile(L, filename)) {
    fprintf(stderr, "Couldn't read config file %s\n", filename);
  } else {
    lua_getglobal(L, "username");
    int idx = lua_gettop(L);
    settings->username = strclone(lua_tostring(L, idx));
  }
}

bool use_config(const char *filename) {
  struct stat buf;
  if(stat(filename, &buf) == 0) {
    return true;
  } else {
    if (!strcmp(filename, DEFAULT_CONFIG_LOCATION)) {
      fprintf(stderr, "Couldn't read config file %s: %s\n", filename, strerror(errno));
    }
    return false;
  }
}

settings conch_load_config(const char *filename) {
  settings settings = {
    .page_size = DEFAULT_PAGE_SIZE
  };

  char *config_path = expand_home(filename);
  if (use_config(config_path)) {
    parse_config(config_path, &settings);
  }
  free(config_path);

  return settings;
}
