CC = gcc
CFLAGS = -Wall -Wextra -pedantic -O2
TEST_CFLAGS = $(CFLAGS) $(shell pkg-config --cflags criterion)
TEST_LDFLAGS = $(shell pkg-config --libs criterion)

SRC = brainfuck.c brainfuck_lib.c
LIB_SRC = brainfuck_lib.c
EXE = bf
TEST_EXE = test_brainfuck
SIMPLE_TEST_EXE = test_simple_framework
TEST_SRCS = tests/test_brainfuck.c tests/test_integration.c tests/test_error_handling.c
COMPREHENSIVE_TEST_EXE = test_comprehensive

all: $(EXE)

$(EXE): $(SRC)
	$(CC) $(CFLAGS) -o $(EXE) $(SRC)

test: $(COMPREHENSIVE_TEST_EXE)
	./$(COMPREHENSIVE_TEST_EXE)

$(SIMPLE_TEST_EXE): $(LIB_SRC) tests/test_simple_framework.c
	$(CC) $(CFLAGS) -o $(SIMPLE_TEST_EXE) $(LIB_SRC) tests/test_simple_framework.c

$(COMPREHENSIVE_TEST_EXE): $(LIB_SRC) tests/test_comprehensive.c
	$(CC) $(CFLAGS) -o $(COMPREHENSIVE_TEST_EXE) $(LIB_SRC) tests/test_comprehensive.c

test-criterion: $(TEST_EXE)
	./$(TEST_EXE)

$(TEST_EXE): $(LIB_SRC) $(TEST_SRCS)
	$(CC) $(TEST_CFLAGS) -o $(TEST_EXE) $(LIB_SRC) $(TEST_SRCS) $(TEST_LDFLAGS)

test-verbose: $(COMPREHENSIVE_TEST_EXE)
	./$(COMPREHENSIVE_TEST_EXE)

test-coverage: CFLAGS += -fprofile-arcs -ftest-coverage
test-coverage: $(COMPREHENSIVE_TEST_EXE)
	./$(COMPREHENSIVE_TEST_EXE)
	gcov brainfuck_lib.c
	@echo "Coverage report generated. Check brainfuck_lib.c.gcov"

.PHONY: clean test test-criterion test-verbose test-coverage

clean:
	rm -f $(EXE) $(TEST_EXE) $(SIMPLE_TEST_EXE) $(COMPREHENSIVE_TEST_EXE) *.gcov *.gcda *.gcno debug_test

