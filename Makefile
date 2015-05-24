CFLAGS?=--std=c99 -Wall -Wformat -Werror --pedantic
LDFLAGS?=
DEPS?=.deps

LIBS=libpq
CHECK_LIBS=check

NCURSES_CONFIG=$(shell which ncurses5-config || which ncurses5.4-config)

CFLAGS+=$(shell pkg-config --cflags $(LIBS)) $(shell ${NCURSES_CONFIG} --cflags)
LDFLAGS+=$(shell pkg-config --libs $(LIBS)) -L$(shell ${NCURSES_CONFIG} --libdir) $(shell ${NCURSES_CONFIG} --libs)

CHECK_BINS=$(patsubst %.c,%,$(wildcard *_check.c))

default: conch

conch: conch.o
	$(CC) $(LDFLAGS) -o $@ $^

blastlist_check: blastlist.o conchbackend.o
conchbackend_check: conchbackend.o

check: $(CHECK_BINS)
	@for c in $(CHECK_BINS); do \
		./$$c; \
	done

reformat: *.c *.h
	clang-format -i *.h *.c

clean:
	rm -rf conch *.o $(DEPS) $(CHECK_BINS)

%.o: %.c
	@mkdir -p ${DEPS}
	$(CC) $(CFLAGS) -MMD -MF $(DEPS)/$(notdir $(patsubst %.c,%.d,$<)) -o $@ -c $<

%_check: %_check.o
	$(CC) -o $@ $^ $(LDFLAGS) $(shell pkg-config --libs $(CHECK_LIBS))

-include .deps/*.d

.PHONY: default check clean reformat
