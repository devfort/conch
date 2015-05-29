CDEBUG?=-g
CFLAGS?=
LDFLAGS?=
DEPS=.deps

CFLAGS+=$(CDEBUG) --std=c99 -Wall -Wformat -Werror --pedantic -D_GNU_SOURCE

BINS=conch blast
BINS_TEST=$(patsubst %.c,%,$(wildcard *-check.c))

LIBS=libpq caca MagickWand
LIBS_TEST=check

ifndef verbose
  SILENT = @
endif

ifdef coverage
	# Enable coverage reports. May not work correctly on LLVM yet :(
	CFLAGS_COVER = -fprofile-arcs -ftest-coverage -O0
	LDFLAGS_COVER = -lgcov
	CFLAGS += $(CFLAGS_COVER)
	LDFLAGS += $(LDFLAGS_COVER)
endif

# imglib2 requires this define when compiled without X11 support
CFLAGS+=-DX_DISPLAY_MISSING=1

# lua
CFLAGS+=-I/usr/include/lua5.2
# I'm so sorry, apt puts the library in a strange place and pkg-config doesn't find it
LDFLAGS+=$(shell [ -f "/usr/lib/x86_64-linux-gnu/liblua5.2.a" ] && echo "-llua5.2" || echo "-llua")

CFLAGS+=$(shell pkg-config --cflags $(LIBS))
LDFLAGS+=$(shell pkg-config --libs $(LIBS))

# ncurses doesn't seem to ship with .pc files on all platforms, but it does
# seems to be available in system include directories.
LDFLAGS+=$(shell pkg-config --silence-errors --libs ncursesw || \
                 pkg-config --silence-errors --libs ncurses || \
                 echo -lncurses)

# curl doesn't have a package config file, just `curl-config` :(
CFLAGS+=$(shell curl-config --cflags)
LDFLAGS+=$(shell curl-config --libs)

CFLAGS_TEST=$(CFLAGS)
CFLAGS_TEST+=$(shell pkg-config --cflags $(LIBS_TEST))
LDFLAGS_TEST=$(LDFLAGS)
LDFLAGS_TEST+=$(shell pkg-config --libs $(LIBS_TEST))

all: $(BINS)

busy_loop:
	while true; do git pull && (make conch || make clean conch) && ./conch -c conchrc.example -s; sleep 1; done

conch: \
  blastlist.o \
  blast-render.o \
  caca-driver.o \
  colors.o \
  conchview.o \
  conchview-render.o \
  listview.o \
  listview-render.o \
  listview-keys.o \
  detailview.o \
  detailview-render.o \
  detailview-keys.o \
  render.o \
  config.o \
  cli.o \
  backend.o \
  keys.o \
  strutils.o \
  anigif.o \
  anigif-render.o \
  wordwrap.o \
  explode.o \
  webfetcher.o

blast: blast.o backend.o strutils.o config.o explode.o

blast-render-check: blast-render.o blastlist.o strutils.o colors.o
blastlist-check: blastlist.o backend.o strutils.o
backend-check: backend.o strutils.o .testdb
wordwrap-check: wordwrap.o
listview-check: listview.o blastlist.o strutils.o keys.o listview-keys.o detailview-keys.o detailview.o backend.o config.o explode.o
strutils-check: strutils.o
config-check: config.o backend.o strutils.o check-explode.o

logs:
	mkdir -p logs

conch-check: conch psycopg2 .expectdb logs conch_check.py
	LD_LIBRARY_PATH=$(shell pg_config --libdir) venv/bin/python conch_check.py

check: $(BINS_TEST)
	$(SILENT)./tools/runtests $(BINS_TEST) 

PG_BIN_DIR=$(shell pg_config --bindir)

.testdb: rsrc/schema.sql rsrc/data.sql rsrc/add-trigger.sql
	tools/createdb bugle_test
	touch .testdb

.expectdb: rsrc/schema.sql rsrc/data.sql rsrc/add-trigger.sql
	tools/createdb bugle_expect
	touch .expectdb

clean:
	rm -rf *.o $(DEPS) $(BINS) $(BINS_TEST) .testdb .expectdb venv logs

reformat: *.c *.h
	clang-format -i *.h *.c

coverage: *.gcda *.gcno
	gcov $<

%-check.o: %-check.c
	@mkdir -p $(DEPS)
	@echo "CC  $@"
	$(SILENT)$(CC) -o $@ -c $< $(CFLAGS_TEST) -MMD -MF $(DEPS)/$(notdir $(patsubst %.c,%.d,$<))

%-check: %-check.o
	@echo "LD  $@"
	$(SILENT)$(CC) -o $@ $(filter %.o,$^) $(LDFLAGS_TEST)

%.o: %.c
	@mkdir -p $(DEPS)
	@echo "CC  $@"
	$(SILENT)$(CC) -o $@ -c $< $(CFLAGS) -MMD -MF $(DEPS)/$(notdir $(patsubst %.c,%.d,$<))

$(BINS): %: %.o
	@echo "LD  $@"
	$(SILENT)$(CC) -o $@ $^ $(LDFLAGS)

venv:
	virtualenv venv --python=python2.7

venv/lib/python2.7/site-packages/psycopg2: venv
	venv/bin/pip install -i http://pypi.fort/web/simple psycopg2

psycopg2: venv/lib/python2.7/site-packages/psycopg2

-include .deps/*.d

.PHONY: all clean reformat check $(CHECKTASKS)
