conchbackend_check: conchbackend_check.c conchbackend.c conchbackend.h
	gcc --std=c99 conchbackend_check.c conchbackend.c -lpq -lcheck -oconchbackend_check

check: conchbackend_check
	./conchbackend_check

clean:
	rm -f conchbackend_check *.o
