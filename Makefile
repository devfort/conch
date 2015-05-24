CC=clang
CFLAGS?=--std=c99 -Wall -Wformat -Werror --pedantic
LDFLAGS?=
DEPS?=.deps

BINS=conch
BINS_TEST=$(patsubst %.c,%,$(wildcard *_check.c))

LIBS=libpq
LIBS_TEST=check

CFLAGS+=$(shell pkg-config --cflags $(LIBS))
LDFLAGS+=$(shell pkg-config --libs $(LIBS))

# Oh, ncurses. :(
NCURSES_CONFIG=$(shell which ncurses5-config || which ncurses5.4-config)
CFLAGS+=$(shell $(NCURSES_CONFIG) --cflags)
LDFLAGS+=-L$(shell $(NCURSES_CONFIG) --libdir) $(shell $(NCURSES_CONFIG) --libs)

LDFLAGS_TEST=$(LDFLAGS)
LDFLAGS_TEST+=$(shell pkg-config --libs $(LIBS_TEST))

all: $(BINS)

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

clean:
	rm -rf *.o $(DEPS) $(BINS) $(BINS_TEST)

reformat: *.c *.h
	clang-format -i *.h *.c

$(BINS): %: %.o
	@echo "LD  $@"
	@$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	@mkdir -p ${DEPS}
	@echo "CC  $@"
	@$(CC) -o $@ -c $< $(CFLAGS) -MMD -MF $(DEPS)/$(notdir $(patsubst %.c,%.d,$<))

%_check: %_check.o
	@echo "LD  $@"
	@$(CC) -o $@ $^ $(LDFLAGS_TEST)

-include .deps/*.d

.PHONY: all clean reformat check $(CHECKTASKS)
