all: compile run

compile:
	gcc better.c -o bin/better

run:
	bin/better
