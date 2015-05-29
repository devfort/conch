#include <caca.h>
#include <curses.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>

#include <wand/MagickWand.h>

#include "caca-driver.h"
#include "conchview-render.h"
#include "conchview.h"

#define STATUS_MAXLEN 64

static char const *startup_msgs[] = {
  "reconfiguring display...", "reticulating splines...", "contacting titans...",
  "approaching aural passages...", "monitoring plutonium phase changes...",
};

void conch_conchview_render(conchview *v, WINDOW *w, winrect *rect) {
  struct timeval now;
  gettimeofday(&now, NULL);

  if (!v->started) {
    conch_spinner_show();

    int n = 2 * (int)now.tv_sec + ((int)now.tv_usec / 5e5);
    size_t choice = n % (sizeof(startup_msgs) / sizeof(char const *));

    char *pre = "conch loading: ";
    int prelen = strlen(pre);

    char *msg = malloc(STATUS_MAXLEN * sizeof(char));
    strncpy(msg, pre, prelen);
    strncpy(msg + prelen, startup_msgs[choice], STATUS_MAXLEN - prelen);

    conch_status_set(msg);
    free(msg);
  }

  anigif_render_frame(v->gif, w, rect);
}
