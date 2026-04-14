CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -pedantic

OBJS = node.o opstack.o parser.o expr_tree.o

.PHONY: all clean run run_test

all: main test

main: main.c $(OBJS)
	$(CC) $(CFLAGS) -o main main.c $(OBJS)

test: test.c $(OBJS)
	$(CC) $(CFLAGS) -o test test.c $(OBJS)

node.o: node.c node.h
	$(CC) $(CFLAGS) -c node.c -o node.o

opstack.o: opstack.c opstack.h
	$(CC) $(CFLAGS) -c opstack.c -o opstack.o

parser.o: parser.c parser.h opstack.h
	$(CC) $(CFLAGS) -c parser.c -o parser.o

expr_tree.o: expr_tree.c expr_tree.h node.h
	$(CC) $(CFLAGS) -c expr_tree.c -o expr_tree.o

run: main
	./main

run_test: test
	./test

clean:
	rm -f $(OBJS) main test
