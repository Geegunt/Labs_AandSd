CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -pedantic
TARGET = lab4
OBJ = main.o btree.o
TEST_TARGET = test_runner
TEST_OBJ = test.o btree.o

.PHONY: all clean run test

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

main.o: main.c btree.h
	$(CC) $(CFLAGS) -c main.c -o main.o

btree.o: btree.c btree.h
	$(CC) $(CFLAGS) -c btree.c -o btree.o

test.o: test.c btree.h
	$(CC) $(CFLAGS) -c test.c -o test.o

$(TEST_TARGET): $(TEST_OBJ)
	$(CC) $(CFLAGS) $(TEST_OBJ) -o $(TEST_TARGET)

run: $(TARGET)
	./$(TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	rm -f $(OBJ) $(TEST_OBJ) $(TARGET) $(TEST_TARGET)
