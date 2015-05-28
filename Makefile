CDEBUG?=-g
CFLAGS?=
LDFLAGS?=
DEPS=.deps

CFLAGS+=$(CDEBUG) --std=c99 -Wall -Wformat -Werror --pedantic

BINS=conch blast
BINS_TEST=$(patsubst %.c,%,$(wildcard *-check.c))

LIBS=libpq caca imlib2
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
LDFLAGS+=-llua

CFLAGS+=$(shell pkg-config --cflags $(LIBS))
LDFLAGS+=$(shell pkg-config --libs $(LIBS))

# ncurses doesn't seem to ship with .pc files on all platforms, but it does
# seems to be available in system include directories.
LDFLAGS+=$(shell pkg-config --silence-errors --libs ncursesw || \
                 pkg-config --silence-errors --libs ncurses || \
                 echo -lncurses)

CFLAGS_TEST=$(CFLAGS)
CFLAGS_TEST+=$(shell pkg-config --cflags $(LIBS_TEST))
LDFLAGS_TEST=$(LDFLAGS)
LDFLAGS_TEST+=$(shell pkg-config --libs $(LIBS_TEST))

all: $(BINS)

release_the_kraken: conch check
	./conch

conch: \
  blastlist.o \
  blast-render.o \
  caca-driver.o \
  colors.o \
  common-image.o \
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
  wordwrap.o

blast: blast.o backend.o strutils.o config.o

blast-render-check: blast-render.o blastlist.o strutils.o
blastlist-check: blastlist.o backend.o strutils.o
backend-check: backend.o strutils.o .testdb
wordwrap-check: wordwrap.o
listview-check: listview.o blastlist.o strutils.o keys.o listview-keys.o detailview-keys.o detailview.o
strutils-check: strutils.o
config-check: config.o backend.o strutils.o

logs:
	mkdir -p logs

conch-check: conch psycopg2 pexpect .expectdb logs conch_check.py
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

venv/lib/python2.7/site-packages/pexpect: venv
	venv/bin/pip install -i http://pypi.fort/web/simple pexpect

venv/lib/python2.7/site-packages/psycopg2: venv
	venv/bin/pip install -i http://pypi.fort/web/simple psycopg2

psycopg2: venv/lib/python2.7/site-packages/psycopg2

pexpect: venv/lib/python2.7/site-packages/pexpect

-include .deps/*.d

.PHONY: all clean reformat check $(CHECKTASKS) pexpect
