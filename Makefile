make: compile run

compile: main.c
	gcc -o processnb main.c

run:
	./processnb example.nb
	
run_all:
	find . -iname 'example*.nb' -exec ./processnb {} . \;

clean:
	find example_base -iname 'example*.nb' -exec cp {} . \;