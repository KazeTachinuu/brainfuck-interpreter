#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../brainfuck.h"

// Simple test framework
int tests_run = 0;
int tests_passed = 0;

#define TEST(name) void test_##name()
#define ASSERT(condition) do { \
    tests_run++; \
    if (condition) { \
        tests_passed++; \
        printf("✓ %s\n", __func__); \
    } else { \
        printf("✗ %s: Assertion failed at line %d\n", __func__, __LINE__); \
    } \
} while(0)

#define ASSERT_EQ(actual, expected) ASSERT((actual) == (expected))
#define ASSERT_NE(actual, expected) ASSERT((actual) != (expected))

// Test functions
TEST(initialization) {
    brainfuck_state_t state;
    brainfuck_init(&state);
    
    ASSERT_EQ(state.stackp, 0);
    ASSERT_EQ(state.memp, 0);
    ASSERT_EQ(state.c, 0);
    ASSERT_EQ(state.codelength, 0);
}

TEST(increment_operation) {
    brainfuck_state_t state;
    brainfuck_init(&state);
    
    ASSERT_EQ(brainfuck_interpret(&state, "+"), 0);
    ASSERT_EQ(state.array[0], 1);
    
    brainfuck_reset(&state);
    ASSERT_EQ(brainfuck_interpret(&state, "+++"), 0);
    ASSERT_EQ(state.array[0], 3);
}

TEST(decrement_operation) {
    brainfuck_state_t state;
    brainfuck_init(&state);
    
    ASSERT_EQ(brainfuck_interpret(&state, "-"), 0);
    ASSERT_EQ(state.array[0], -1);
    
    brainfuck_reset(&state);
    ASSERT_EQ(brainfuck_interpret(&state, "++-"), 0);
    ASSERT_EQ(state.array[0], 1);
}

TEST(pointer_movement) {
    brainfuck_state_t state;
    brainfuck_init(&state);
    
    ASSERT_EQ(brainfuck_interpret(&state, ">"), 0);
    ASSERT_EQ(state.memp, 1);
    
    brainfuck_reset(&state);
    ASSERT_EQ(brainfuck_interpret(&state, "><"), 0);
    ASSERT_EQ(state.memp, 0);
}

TEST(simple_loops) {
    brainfuck_state_t state;
    brainfuck_init(&state);
    
    ASSERT_EQ(brainfuck_interpret(&state, "[+]"), 0);
    ASSERT_EQ(state.array[0], 0);
    
    brainfuck_reset(&state);
    ASSERT_EQ(brainfuck_interpret(&state, "+[-]"), 0);
    ASSERT_EQ(state.array[0], 0);
}

TEST(bracket_validation) {
    ASSERT_EQ(brainfuck_validate_brackets("[]", 2), 0);
    ASSERT_EQ(brainfuck_validate_brackets("[[]]", 4), 0);
    ASSERT_NE(brainfuck_validate_brackets("]", 1), 0);
    ASSERT_NE(brainfuck_validate_brackets("[", 1), 0);
}

TEST(complex_program) {
    brainfuck_state_t state;
    brainfuck_init(&state);
    
    // Test a program that sets cell 0 to 5
    ASSERT_EQ(brainfuck_interpret(&state, "+++++"), 0);
    ASSERT_EQ(state.array[0], 5);
    
    // Test a program that copies value from cell 0 to cell 1
    brainfuck_reset(&state);
    ASSERT_EQ(brainfuck_interpret(&state, "+++++[>+<-]"), 0);
    ASSERT_EQ(state.array[0], 0);
    ASSERT_EQ(state.array[1], 5);
}

int main() {
    printf("Running Brainfuck Test Suite\n");
    printf("============================\n\n");
    
    test_initialization();
    test_increment_operation();
    test_decrement_operation();
    test_pointer_movement();
    test_simple_loops();
    test_bracket_validation();
    test_complex_program();
    
    printf("\n============================\n");
    printf("Tests run: %d\n", tests_run);
    printf("Tests passed: %d\n", tests_passed);
    printf("Tests failed: %d\n", tests_run - tests_passed);
    
    if (tests_passed == tests_run) {
        printf("All tests passed! ✓\n");
        return 0;
    } else {
        printf("Some tests failed! ✗\n");
        return 1;
    }
}