#include <stdlib.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "config.h"
#include "strutils.h"

void parse_config(const char *filename, settings *settings) {
  lua_State *L = luaL_newstate();
  luaL_dofile(L, filename);

  lua_getglobal(L, "username");
  int idx = lua_gettop(L);
  settings->username = strclone(lua_tostring(L, idx));
}

settings conch_load_config(const char *filename) {
  settings settings = {
    .page_size = DEFAULT_PAGE_SIZE
  };

  char *config_path = expand_home(filename);
  parse_config(config_path, &settings);
  free(config_path);

  return settings;
}
