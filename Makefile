CFLAGS?=--std=c99 -Wall -Wformat -Werror --pedantic
LDFLAGS?=
DEPS?=.deps

LIBS=libpq ncurses
CHECK_LIBS=check

CFLAGS+=$(shell pkg-config --cflags $(LIBS))
LDFLAGS+=$(shell pkg-config --libs $(LIBS))

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
	$(CC) $(CFLAGS) -MMD -MF ${DEPS}/$(notdir $(patsubst %.c,%.d,$<)) -o $@ -c $<

%_check: %_check.o
	$(CC) $(LDFLAGS) $(shell pkg-config --libs $(CHECK_LIBS)) -o $@ $^

-include .deps/*.d

.PHONY: default check clean reformat
