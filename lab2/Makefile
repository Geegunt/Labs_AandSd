CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -pedantic
TARGET = lab2
TEST_TARGET = test
OBJ = main.o sort.o
TEST_OBJ = test.o sort.o

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

main.o: main.c sort.h
	$(CC) $(CFLAGS) -c main.c

sort.o: sort.c sort.h
	$(CC) $(CFLAGS) -c sort.c

test.o: test.c sort.h test.h
	$(CC) $(CFLAGS) -c test.c

test: $(TEST_OBJ)
	$(CC) $(CFLAGS) $(TEST_OBJ) -o $(TEST_TARGET)
	./$(TEST_TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJ) test.o $(TARGET) $(TEST_TARGET)
