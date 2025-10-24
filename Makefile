CC = gcc
CFLAGS = -Wall -Wextra -pedantic -O2
TEST_CFLAGS = $(CFLAGS) $(shell pkg-config --cflags criterion)
TEST_LDFLAGS = $(shell pkg-config --libs criterion)

SRC = brainfuck.c
EXE = bf
TEST_EXE = test_brainfuck
TEST_SRCS = tests/test_brainfuck.c tests/test_integration.c tests/test_error_handling.c

all: $(EXE)

$(EXE): $(SRC)
	$(CC) $(CFLAGS) -o $(EXE) $(SRC)

test: $(TEST_EXE)
	./$(TEST_EXE)

$(TEST_EXE): $(SRC) $(TEST_SRCS)
	$(CC) $(TEST_CFLAGS) -o $(TEST_EXE) $(SRC) $(TEST_SRCS) $(TEST_LDFLAGS)

test-verbose: $(TEST_EXE)
	./$(TEST_EXE) --verbose

test-coverage: CFLAGS += -fprofile-arcs -ftest-coverage
test-coverage: TEST_CFLAGS += -fprofile-arcs -ftest-coverage
test-coverage: $(TEST_EXE)
	./$(TEST_EXE)
	gcov brainfuck.c
	@echo "Coverage report generated. Check brainfuck.c.gcov"

.PHONY: clean test test-verbose test-coverage

clean:
	rm -f $(EXE) $(TEST_EXE) *.gcov *.gcda *.gcno

