conch: conch.c
	gcc --std=c99 conch.c -lncurses -oconch

conchbackend_check: conchbackend_check.c conchbackend.c conchbackend.h
	gcc -Wall -Werror -pedantic --std=c99 conchbackend_check.c conchbackend.c -lpq -lcheck -oconchbackend_check

check: conchbackend_check
	./conchbackend_check

reformat: *.c *.h
	clang-format -i *.h *.c

clean:
	rm -f conchbackend_check *.o
