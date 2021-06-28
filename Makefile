all:
	gcc *.c -o main

run:
	./main

debug:
	valgrind ./main 