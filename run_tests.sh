#!/bin/bash

# Brainfuck Test Runner
# This script runs the test suite with various options

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to show usage
show_usage() {
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  -h, --help          Show this help message"
    echo "  -v, --verbose       Run tests in verbose mode"
    echo "  -c, --coverage      Run tests with coverage analysis"
    echo "  -q, --quiet         Run tests in quiet mode"
    echo "  -f, --fast          Run only fast tests (skip integration tests)"
    echo "  --clean             Clean build artifacts before running tests"
    echo "  --rebuild           Rebuild test executable before running"
    echo ""
    echo "Examples:"
    echo "  $0                  # Run all tests"
    echo "  $0 --verbose        # Run tests with detailed output"
    echo "  $0 --coverage       # Run tests and generate coverage report"
    echo "  $0 --fast           # Run only unit tests"
}

# Default options
VERBOSE=false
COVERAGE=false
QUIET=false
FAST=false
CLEAN=false
REBUILD=false

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            show_usage
            exit 0
            ;;
        -v|--verbose)
            VERBOSE=true
            shift
            ;;
        -c|--coverage)
            COVERAGE=true
            shift
            ;;
        -q|--quiet)
            QUIET=true
            shift
            ;;
        -f|--fast)
            FAST=true
            shift
            ;;
        --clean)
            CLEAN=true
            shift
            ;;
        --rebuild)
            REBUILD=true
            shift
            ;;
        *)
            print_error "Unknown option: $1"
            show_usage
            exit 1
            ;;
    esac
done

# Clean if requested
if [ "$CLEAN" = true ]; then
    print_status "Cleaning build artifacts..."
    make clean
fi

# Build test executable
if [ "$REBUILD" = true ] || [ ! -f "test_brainfuck" ]; then
    print_status "Building test executable..."
    if [ "$COVERAGE" = true ]; then
        make test-coverage
    else
        make test
    fi
fi

# Check if test executable exists
if [ ! -f "test_brainfuck" ]; then
    print_error "Test executable not found. Run 'make test' first."
    exit 1
fi

# Prepare test command
TEST_CMD="./test_brainfuck"

if [ "$VERBOSE" = true ]; then
    TEST_CMD="$TEST_CMD --verbose"
fi

if [ "$QUIET" = true ]; then
    TEST_CMD="$TEST_CMD --quiet"
fi

# Run tests
print_status "Running Brainfuck test suite..."

if [ "$FAST" = true ]; then
    print_warning "Running in fast mode (skipping integration tests)"
    # Run only unit tests by filtering out integration tests
    $TEST_CMD --filter="!integration/*"
else
    $TEST_CMD
fi

# Check test result
if [ $? -eq 0 ]; then
    print_success "All tests passed!"
    
    if [ "$COVERAGE" = true ]; then
        print_status "Coverage report generated. Check brainfuck.c.gcov for details."
    fi
else
    print_error "Some tests failed!"
    exit 1
fi