all: better run

better:
	gcc better.c -o bin/better

run:
	bin/better
