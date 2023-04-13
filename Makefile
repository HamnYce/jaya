single: compile_single run_single

kpop: compile_kpop run_kpop

compile_kpop:
	gcc -lpthread kpop.c lib/helper.c -o bin/kpop

run_kpop:
	bin/kpop

compile_single:
	gcc better.c lib/helper.c -o bin/better

run_single:
	bin/better
