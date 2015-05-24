CFLAGS?=--std=c99 -Wall -Wformat -Werror --pedantic
LDFLAGS?=-lpq
DEPS?=.deps

CHECK_BINS=$(patsubst %.c,%,$(wildcard *_check.c))

default: conch

conch: conch.o
	$(CC) $(LDFLAGS) -lncurses -o $@ $^

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
	$(CC) $(LDFLAGS) -lcheck -o $@ $^

-include .deps/*.d

.PHONY: default check clean reformat
