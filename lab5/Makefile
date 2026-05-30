CC       = gcc
CXX      = g++
CFLAGS   = -Wall -Wextra -Werror -std=c11 -pedantic
CXXFLAGS = -Wall -Wextra -std=c++17 -I/opt/homebrew/include
TARGET   = lab5
TEST_TARGET  = test_runner
BENCH_TARGET = benchmark

GTEST_FLAGS = -L/opt/homebrew/lib -lgtest -lgtest_main -lpthread

OBJ       = main.o graph.o
TEST_OBJ  = test_floyd.o graph.o
BENCH_OBJ = benchmark.o graph.o

.PHONY: all clean run test bench

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

main.o: main.c graph.h
	$(CC) $(CFLAGS) -c main.c -o main.o

graph.o: graph.c graph.h
	$(CC) $(CFLAGS) -c graph.c -o graph.o

test_floyd.o: test_floyd.cpp graph.h
	$(CXX) $(CXXFLAGS) -c test_floyd.cpp -o test_floyd.o

$(TEST_TARGET): test_floyd.o graph.o
	$(CXX) $(CXXFLAGS) test_floyd.o graph.o $(GTEST_FLAGS) -o $(TEST_TARGET)

benchmark.o: benchmark.c graph.h
	$(CC) $(CFLAGS) -c benchmark.c -o benchmark.o

$(BENCH_TARGET): $(BENCH_OBJ)
	$(CC) $(CFLAGS) $(BENCH_OBJ) -o $(BENCH_TARGET)

run: $(TARGET)
	./$(TARGET) 5

test: $(TEST_TARGET)
	./$(TEST_TARGET)

bench: $(BENCH_TARGET)
	./$(BENCH_TARGET)

clean:
	rm -f *.o $(TARGET) $(TEST_TARGET) $(BENCH_TARGET) output.txt
