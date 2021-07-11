CC=gcc
LD=gcc

CFLAGS=-Wall -Wextra -Werror -pedantic -g -O0
LDLIBS=-lm
DLDFLAGS=-g
LDFLAGS=

VFLAGS=--leak-check=full --show-leak-kinds=all --track-origins=yes

SRCS    := $(wildcard ./*.c)
OBJS    := $(patsubst ./%.c,./%.o,$(SRCS))

TARGET=main

./%.o: ./%.c ./%.h
	$(CC) $(CFLAGS) -c $< -o $@

all: $(OBJS)
	$(LD) $(LDFLAGS) $^ -o $(TARGET) $(LDLIBS)

debug: $(OBJS)
	$(LD) $(DLDFLAGS) $(OBJS) -o $(TARGET) $(LDLIBS)

clean:
	rm -rf $(OBJS) $(TARGET)

hardClean:
	rm -rf $(OBJS) $(TARGET) $(TARGET).zip *.c *.h *.in *.out vgcore* in out README.txt

run: all
	./$(TARGET)

leaks: debug
	leaks -atExit -- ./$(TARGET)

valgrind: debug
	valgrind $(VFLAGS) ./$(TARGET)

zip:
	zip -r $(TARGET).zip Makefile *.h *.c