CDEBUG?=-g
CFLAGS?=
LDFLAGS?=
DEPS=.deps

CFLAGS+=$(CDEBUG) --std=c99 -Wall -Wformat -Werror --pedantic

BINS=conch
BINS_TEST=$(patsubst %.c,%,$(wildcard *_check.c))

LIBS=libpq caca imlib2
LIBS_TEST=check

ifndef verbose
  SILENT = @
endif

# imglib2 requires this define when compiled without X11 support
CFLAGS+=-DX_DISPLAY_MISSING=1

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
  caca-driver.o \
  colors.o \
  common-image.o \
  conchbackend.o \
  listview.o \
  listview_render.o \
  strutils.o \
  wordwrap.o

blastlist_check: blastlist.o conchbackend.o strutils.o
conchbackend_check: conchbackend.o strutils.o .testdb
wordwrap_check: wordwrap.o
listview_check: listview.o blastlist.o strutils.o

CHECKTASKS=$(patsubst %_check,check_%,$(BINS_TEST))
check: $(BINS_TEST)
	$(SILENT)set -e && for t in $^; do \
		./$$t | ./greenify; \
		echo ----; \
	done

PG_BIN_DIR=$(shell pg_config --bindir)

.testdb: schema.sql data.sql
	$(PG_BIN_DIR)/dropdb -h localhost bugle_test 2>/dev/null || true
	$(PG_BIN_DIR)/dropuser -h localhost bugle 2>/dev/null || true
	$(PG_BIN_DIR)/createuser -h localhost -SDR bugle
	$(PG_BIN_DIR)/createdb -h localhost bugle_test --owner=bugle
	psql -h localhost bugle_test < schema.sql
	psql -h localhost bugle_test < data.sql
	touch .testdb

clean:
	rm -rf *.o $(DEPS) $(BINS) $(BINS_TEST)

reformat: *.c *.h
	clang-format -i *.h *.c

%_check.o: %_check.c
	@mkdir -p $(DEPS)
	@echo "CC  $@"
	$(SILENT)$(CC) -o $@ -c $< $(CFLAGS_TEST) -MMD -MF $(DEPS)/$(notdir $(patsubst %.c,%.d,$<))

%_check: %_check.o
	@echo "LD  $@"
	$(SILENT)$(CC) -o $@ $(filter %.o,$^) $(LDFLAGS_TEST)

%.o: %.c
	@mkdir -p $(DEPS)
	@echo "CC  $@"
	$(SILENT)$(CC) -o $@ -c $< $(CFLAGS) -MMD -MF $(DEPS)/$(notdir $(patsubst %.c,%.d,$<))

$(BINS): %: %.o
	@echo "LD  $@"
	$(SILENT)$(CC) -o $@ $^ $(LDFLAGS)

-include .deps/*.d

.PHONY: all clean reformat check $(CHECKTASKS)
