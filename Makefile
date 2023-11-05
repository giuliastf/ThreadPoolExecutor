all : build
build :
	gcc -g -Wall tema2.c -o tema2
run :
	./tema2
clean :
	rm -rf tema2