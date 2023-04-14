single:
	make compile_single
	make run_single

kpop:	
	make compile_kpop
	make run_kpop

compile_kpop:
	gcc kpop.c lib/helper.c -lpthread -o bin/kpop $(POP_S) $(D) $(MAXFE)

run_kpop:
	bin/kpop

compile_single:
	gcc single.c lib/helper.c -o bin/single $(POP_S) $(D) $(MAXFE)

run_single:
	bin/single
