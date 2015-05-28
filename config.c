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

static void parse_config(const char *filename, settings *settings) {
  int idx;
  lua_State *L = luaL_newstate();
  if (luaL_dofile(L, filename)) {
    fprintf(stderr, "Couldn't read config file %s\n", filename);
  } else {
    struct {
      char *var;
      char **dest;
    } vars[] = {
      { "username", &settings->username },
      { "host", &settings->host },
      { "database", &settings->database },
    };
    for (int i = 0; i < 3; i++) {
      lua_getglobal(L, vars[i].var);
      idx = lua_gettop(L);
      *vars[i].dest = strclone(lua_tostring(L, idx));
    }

    lua_getglobal(L, "page_size");
    idx = lua_gettop(L);
    const char *page_size = lua_tostring(L, idx);
    if (page_size != NULL) {
      settings->page_size = atoi(page_size);
    }
  }
}

static bool use_config(const char *filename) {
  struct stat buf;
  if (stat(filename, &buf) == 0) {
    return true;
  } else {
    if (!strcmp(filename, DEFAULT_CONFIG_LOCATION)) {
      fprintf(stderr, "Couldn't read config file %s: %s\n", filename,
              strerror(errno));
    }
    return false;
  }
}

settings conch_load_config(const char *filename) {
  settings settings = {.page_size = DEFAULT_PAGE_SIZE };

  char *config_path = expand_home(filename);
  if (use_config(config_path)) {
    parse_config(config_path, &settings);
  }
  free(config_path);

  return settings;
}
