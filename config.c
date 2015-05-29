#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <time.h>
#include <setjmp.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "config.h"
#include "strutils.h"

static lua_State *L;
static bool use_lua = false;

jmp_buf savepoint;

static int everybody_panic(lua_State *L) {
  longjmp(savepoint, 1);
}

void generate_clock_text(int time_str_limit,
                                char *time_str) {
  if (use_lua) {
    lua_getglobal(L, "clock_format_func");
    int idx = lua_gettop(L);
    if (lua_isfunction(L, idx)) {
      if (setjmp(savepoint) == 0) {
        lua_call(L, 0, 1);
        const char *result = lua_tostring(L, lua_gettop(L));
        snprintf(time_str, time_str_limit, " %s ", result);
        lua_pop(L, 1);
        return;
      }
    } else {
      lua_pop(L, 1);
    }
  }

  time_t now = time(NULL);
  struct tm *now_tm = localtime(&now);
  strftime(time_str, time_str_limit, " %Y-%m-%d %H:%M:%S ", now_tm);
}

static void parse_config(const char *filename, settings *settings) {
  int idx;
  use_lua = true;
  L = luaL_newstate();

  lua_CFunction panic = everybody_panic;
  lua_atpanic(L, panic);
  
  luaL_openlibs(L);
  if (luaL_dofile(L, filename)) {
    //TODO: change to fatal_error
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

void conch_free_config_state() {
  if (L != NULL) {
    lua_close(L);
  }
}
