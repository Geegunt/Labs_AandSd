all: main.o tree.o
	gcc -o main main.o tree.o

main.o: main.c tree.h
	gcc -c main.c

tree.o: tree.c tree.h
	gcc -c tree.c

test.o: test.c tree.h test.h
	gcc -c test.c

test: tree.o test.o
	gcc -o test tree.o test.o
	./test

clean:
	rm -f *.o main test