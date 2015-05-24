conchbackend_check: conchbackend_check.c conchbackend.c conchbackend.c
	gcc conchbackend_check.c conchbackend.c -lcheck -oconchbackend_check

check: conchbackend_check
	./conchbackend_check
