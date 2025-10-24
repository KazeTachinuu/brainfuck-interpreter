# Brainfuck Interpreter

This is a simple Brainfuck interpreter written in C.

## Table of Contents

- [Introduction](#introduction)
- [Usage](#usage)
- [Building](#building)


## Introduction

Brainfuck is an esoteric programming language created in 1993 by Urban Müller. It is known for its extreme minimalism, with the language using only eight simple commands, an instruction pointer, and an array of memory cells, each initialized to zero.

This project provides a basic Brainfuck interpreter that can execute Brainfuck programs.

## Usage

To run a Brainfuck program:

```bash
./bf <filename>
```
Replace `<filename>` with the path to the Brainfuck program file you want to execute.

You can also run a Brainfuck program directly from the command line using the `-c` option:

```bash
./bf -c "++++++++++[>+++++++>++++++++>+++<<<-]>+."
```

## Building

To build the Brainfuck interpreter, use the provided `Makefile`:

```bash
make
```
This will generate the executable named `bf`.

## Testing

This project includes a comprehensive test suite using the Criterion testing framework. The test suite covers:

- **Unit Tests**: Individual Brainfuck operations (increment, decrement, pointer movement, loops)
- **Integration Tests**: Full program execution using example Brainfuck programs
- **Error Handling Tests**: Edge cases, invalid input, and boundary conditions
- **Memory Management Tests**: Memory boundary protection and overflow handling

### Running Tests

```bash
# Run all tests
make test

# Run tests with verbose output
make test-verbose

# Run tests with coverage analysis
make test-coverage

# Or use the test runner script
./run_tests.sh

# Run only fast tests (skip integration tests)
./run_tests.sh --fast

# Run tests with coverage analysis
./run_tests.sh --coverage
```

### Test Structure

- `tests/test_comprehensive.c` - Comprehensive unit tests for all Brainfuck operations
- `tests/test_integration.c` - Integration tests using example programs
- `tests/test_error_handling.c` - Error handling and edge case tests
- `tests/test_simple_framework.c` - Basic test framework implementation

The test suite includes **175 individual test cases** covering:
- All 8 Brainfuck commands (`+`, `-`, `<`, `>`, `[`, `]`, `.`, `,`)
- Memory management and boundary conditions
- Loop processing (simple, nested, and complex)
- Mathematical programs (addition, multiplication)
- Input/output operations with file and string input
- Error conditions and malformed programs
- Performance testing with large programs
- Memory safety and cleanup validation

### Test Results

The comprehensive test suite validates:
- ✅ **Basic Operations**: Increment, decrement, pointer movement
- ✅ **Memory Management**: Dynamic allocation, boundary protection
- ✅ **Loop Processing**: All loop types and nesting levels
- ✅ **Mathematical Programs**: Complex algorithms and calculations
- ✅ **Input/Output**: File loading, string input, output capture
- ✅ **Error Handling**: Invalid input, memory overflow, edge cases
- ✅ **Performance**: Large programs and stress testing

For detailed test information, see [TEST_SUMMARY.md](TEST_SUMMARY.md).

## Adding the Executable to PATH 

To use the Brainfuck interpreter globally and access it from anywhere, you can add the executable to your system's PATH.

You can move the executable in /bin:

```bash
sudo mv bf /bin/
```
