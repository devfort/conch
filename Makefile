CFLAGS?=--std=c99 -Wall -Wformat -Werror --pedantic
LDFLAGS?=
DEPS?=.deps

BINS=conch
BINS_TEST=$(patsubst %.c,%,$(wildcard *_check.c))

LIBS=libpq
LIBS_TEST=check

CFLAGS+=$(shell pkg-config --cflags $(LIBS))
LDFLAGS+=$(shell pkg-config --libs $(LIBS))

# ncurses doesn't seem to ship with .pc files on all platforms, but it does
# seems to be available in system include directories.
LDFLAGS+=-lncurses

CFLAGS_TEST=$(CFLAGS)
CFLAGS_TEST+=$(shell pkg-config --cflags $(LIBS_TEST))
LDFLAGS_TEST=$(LDFLAGS)
LDFLAGS_TEST+=$(shell pkg-config --libs $(LIBS_TEST))

all: $(BINS)

conch: blastlist.o conchbackend.o

blastlist_check: blastlist.o conchbackend.o
conchbackend_check: conchbackend.o

CHECKTASKS=$(patsubst %_check,check_%,$(BINS_TEST))
check: $(BINS_TEST)
	@set -e && for t in $^; do \
		./$$t; \
		echo ----; \
	done
check_%: %_check
	@./$<

PG_BIN_DIR=$(shell pg_config --bindir)

.testdb: schema.sql data.sql
	$(PG_BIN_DIR)/dropdb bugle_test 2>/dev/null || true
	$(PG_BIN_DIR)/dropuser bugle 2>/dev/null || true
	$(PG_BIN_DIR)/createuser -SDR bugle
	$(PG_BIN_DIR)/createdb bugle_test --owner=bugle
	psql bugle_test < schema.sql
	psql bugle_test < data.sql
	touch .testdb

clean:
	rm -rf *.o $(DEPS) $(BINS) $(BINS_TEST)

reformat: *.c *.h
	clang-format -i *.h *.c

$(BINS): %: %.o
	@echo "LD  $@"
	@$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	@mkdir -p $(DEPS)
	@echo "CC  $@"
	@$(CC) -o $@ -c $< $(CFLAGS) -MMD -MF $(DEPS)/$(notdir $(patsubst %.c,%.d,$<))

%_check.o: %_check.c
	@mkdir -p $(DEPS)
	@echo "CC  $@"
	@$(CC) -o $@ -c $< $(CFLAGS_TEST) -MMD -MF $(DEPS)/$(notdir $(patsubst %.c,%.d,$<))

%_check: %_check.o
	@echo "LD  $@"
	@$(CC) -o $@ $^ $(LDFLAGS_TEST)

-include .deps/*.d

.PHONY: all clean reformat check $(CHECKTASKS)
