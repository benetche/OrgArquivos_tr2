all:
	gcc *.c *.h -o main -Wall -Werror

run:
	./main

debug:
	valgrind ./main 