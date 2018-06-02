make: compile run

compile: main.c
	gcc -Wall main.c lib/helpers.c -o notebook

run:
	./notebook example.nb
	
run_all: compile
	find . -maxdepth 1 -iname 'example*.nb' -exec ./notebook {} . \;

clean:
	find example_base -maxdepth 1 -iname 'example*.nb' -exec cp {} . \;
