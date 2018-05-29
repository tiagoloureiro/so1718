make: compile run

compile: main.c
	@gcc -Wall -o2 main.c lib/helpers.c -o processnb

run:
	@./processnb example.nb
	
run_all: compile
	@find . -maxdepth 1 -iname 'example*.nb' -exec ./processnb {} . \;

clean:
	@find example_base -iname 'example*.nb' -exec cp {} . \;