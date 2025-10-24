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

// Test basic operations
TEST(basic_increment) {
    brainfuck_state_t state;
    brainfuck_init(&state);
    
    ASSERT_EQ(brainfuck_interpret(&state, "+"), 0);
    ASSERT_EQ(state.array[0], 1);
    
    brainfuck_cleanup(&state);
}

TEST(basic_decrement) {
    brainfuck_state_t state;
    brainfuck_init(&state);
    
    ASSERT_EQ(brainfuck_interpret(&state, "-"), 0);
    ASSERT_EQ(state.array[0], -1);
    
    brainfuck_cleanup(&state);
}

TEST(pointer_movement) {
    brainfuck_state_t state;
    brainfuck_init(&state);
    
    ASSERT_EQ(brainfuck_interpret(&state, ">"), 0);
    ASSERT_EQ(state.memp, 1);
    
    ASSERT_EQ(brainfuck_interpret(&state, "<"), 0);
    ASSERT_EQ(state.memp, 0);
    
    brainfuck_cleanup(&state);
}

TEST(pointer_boundaries) {
    brainfuck_state_t state;
    brainfuck_init(&state);
    
    // Test left boundary
    ASSERT_EQ(brainfuck_interpret(&state, "<"), 0);
    ASSERT_EQ(state.memp, 0);
    
    // Test right boundary (move to end)
    for (int i = 0; i < ARRAYSIZE - 1; i++) {
        ASSERT_EQ(brainfuck_interpret(&state, ">"), 0);
    }
    ASSERT_EQ(state.memp, ARRAYSIZE - 1);
    
    // Try to go beyond
    ASSERT_EQ(brainfuck_interpret(&state, ">"), 0);
    ASSERT_EQ(state.memp, ARRAYSIZE - 1);
    
    brainfuck_cleanup(&state);
}

TEST(simple_loops) {
    brainfuck_state_t state;
    brainfuck_init(&state);
    
    // Loop that doesn't execute
    ASSERT_EQ(brainfuck_interpret(&state, "[+]"), 0);
    ASSERT_EQ(state.array[0], 0);
    
    // Loop that executes once
    brainfuck_reset(&state);
    ASSERT_EQ(brainfuck_interpret(&state, "+[-]"), 0);
    ASSERT_EQ(state.array[0], 0);
    
    // Loop that executes multiple times
    brainfuck_reset(&state);
    ASSERT_EQ(brainfuck_interpret(&state, "+++[-]"), 0);
    ASSERT_EQ(state.array[0], 0);
    
    brainfuck_cleanup(&state);
}

TEST(nested_loops) {
    brainfuck_state_t state;
    brainfuck_init(&state);
    
    // Simple nested loop
    ASSERT_EQ(brainfuck_interpret(&state, "+++[>+[>+<-]<-]"), 0);
    ASSERT_EQ(state.array[0], 0);
    ASSERT_EQ(state.array[1], 0);
    ASSERT_EQ(state.array[2], 3);
    
    brainfuck_cleanup(&state);
}

TEST(complex_loops) {
    brainfuck_state_t state;
    brainfuck_init(&state);
    
    // Copy value from cell 0 to cell 1
    ASSERT_EQ(brainfuck_interpret(&state, "+++++[>+<-]"), 0);
    ASSERT_EQ(state.array[0], 0);
    ASSERT_EQ(state.array[1], 5);
    
    brainfuck_cleanup(&state);
}

TEST(addition_program) {
    brainfuck_state_t state;
    brainfuck_init(&state);
    
    // Add 3 + 2 = 5
    ASSERT_EQ(brainfuck_interpret(&state, "+++>++<[>+<-]"), 0);
    ASSERT_EQ(state.array[0], 0);
    ASSERT_EQ(state.array[1], 5);
    
    brainfuck_cleanup(&state);
}

TEST(multiplication_program) {
    brainfuck_state_t state;
    brainfuck_init(&state);
    
    // Multiply 3 * 2 = 6
    ASSERT_EQ(brainfuck_interpret(&state, "+++>++<[>[>+>+<<-]>>[<<+>>-]<<<-]"), 0);
    ASSERT_EQ(state.array[0], 0);
    ASSERT_EQ(state.array[1], 0);
    ASSERT_EQ(state.array[2], 6);
    
    brainfuck_cleanup(&state);
}

TEST(bracket_validation) {
    // Valid brackets
    ASSERT_EQ(brainfuck_validate_brackets("[]", 2), 0);
    ASSERT_EQ(brainfuck_validate_brackets("[[]]", 4), 0);
    ASSERT_EQ(brainfuck_validate_brackets("[[[]]]", 6), 0);
    ASSERT_EQ(brainfuck_validate_brackets("+++[>+++<-]", 11), 0);
    ASSERT_EQ(brainfuck_validate_brackets("", 0), 0);
    ASSERT_EQ(brainfuck_validate_brackets("++++", 4), 0);
    
    // Invalid brackets
    ASSERT_NE(brainfuck_validate_brackets("]", 1), 0);
    ASSERT_NE(brainfuck_validate_brackets("[", 1), 0);
    ASSERT_NE(brainfuck_validate_brackets("][", 2), 0);
    ASSERT_NE(brainfuck_validate_brackets("[[", 2), 0);
    ASSERT_NE(brainfuck_validate_brackets("]]", 2), 0);
    ASSERT_NE(brainfuck_validate_brackets("[[]", 3), 0);
}

TEST(input_handling) {
    brainfuck_state_t state;
    brainfuck_init(&state);
    
    // Test input with predefined string
    ASSERT_EQ(brainfuck_interpret_with_input(&state, ",", "A"), 0);
    ASSERT_EQ(state.array[0], 65); // ASCII 'A'
    
    // Test multiple inputs
    brainfuck_reset(&state);
    ASSERT_EQ(brainfuck_interpret_with_input(&state, ",,", "AB"), 0);
    ASSERT_EQ(state.array[0], 65); // ASCII 'A'
    ASSERT_EQ(state.array[1], 66); // ASCII 'B'
    
    brainfuck_cleanup(&state);
}

TEST(memory_operations) {
    brainfuck_state_t state;
    brainfuck_init(&state);
    
    // Test setting values at different positions
    ASSERT_EQ(brainfuck_interpret(&state, ">+>++>+++"), 0);
    ASSERT_EQ(state.array[1], 1);
    ASSERT_EQ(state.array[2], 2);
    ASSERT_EQ(state.array[3], 3);
    ASSERT_EQ(state.memp, 3);
    
    // Test reading values
    ASSERT_EQ(brainfuck_get_memory_value(&state, 1), 1);
    ASSERT_EQ(brainfuck_get_memory_value(&state, 2), 2);
    ASSERT_EQ(brainfuck_get_memory_value(&state, 3), 3);
    ASSERT_EQ(brainfuck_get_memory_value(&state, -1), -1); // Invalid position
    ASSERT_EQ(brainfuck_get_memory_value(&state, ARRAYSIZE), -1); // Invalid position
    
    brainfuck_cleanup(&state);
}

TEST(reset_functionality) {
    brainfuck_state_t state;
    brainfuck_init(&state);
    
    // Set some values
    ASSERT_EQ(brainfuck_interpret(&state, "+++>>+"), 0);
    ASSERT_EQ(state.array[0], 3);
    ASSERT_EQ(state.array[2], 1);
    ASSERT_EQ(state.memp, 2);
    
    // Reset and verify
    brainfuck_reset(&state);
    ASSERT_EQ(state.memp, 0);
    ASSERT_EQ(state.array[0], 0);
    ASSERT_EQ(state.array[2], 0);
    
    brainfuck_cleanup(&state);
}

TEST(edge_cases) {
    brainfuck_state_t state;
    brainfuck_init(&state);
    
    // Empty program
    ASSERT_EQ(brainfuck_interpret(&state, ""), 0);
    
    // Program with only non-command characters
    ASSERT_EQ(brainfuck_interpret(&state, "abc123!@#"), 0);
    
    // Program with mixed commands and non-commands
    ASSERT_EQ(brainfuck_interpret(&state, "a+b-c<d>e,f.g[h]i"), 0);
    ASSERT_EQ(state.array[0], 0); // Should be 0 after +-
    ASSERT_EQ(state.memp, 0); // Should be 0 after < >
    
    brainfuck_cleanup(&state);
}

TEST(performance_large_program) {
    brainfuck_state_t state;
    brainfuck_init(&state);
    
    // Create a large program with many operations
    char large_program[1000];
    int pos = 0;
    
    // Add 100 increments
    for (int i = 0; i < 100; i++) {
        large_program[pos++] = '+';
    }
    
    // Add a loop that decrements 50 times
    large_program[pos++] = '[';
    for (int i = 0; i < 50; i++) {
        large_program[pos++] = '-';
    }
    large_program[pos++] = ']';
    
    large_program[pos] = '\0';
    
    ASSERT_EQ(brainfuck_interpret(&state, large_program), 0);
    ASSERT_EQ(state.array[0], 50);
    
    brainfuck_cleanup(&state);
}

TEST(hello_world_program) {
    brainfuck_state_t state;
    brainfuck_init(&state);
    
    // Classic "Hello World!" program
    const char* hello_world = "++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]>>.>---.+++++++..+++.>>.<-.<.+++.------.--------.>>+.>++.";
    
    ASSERT_EQ(brainfuck_interpret(&state, hello_world), 0);
    // Note: We can't easily test the output in this simple framework
    // but we can verify the program executes without error
    
    brainfuck_cleanup(&state);
}

int main() {
    printf("Running Comprehensive Brainfuck Test Suite\n");
    printf("==========================================\n\n");
    
    test_basic_increment();
    test_basic_decrement();
    test_pointer_movement();
    test_pointer_boundaries();
    test_simple_loops();
    test_nested_loops();
    test_complex_loops();
    test_addition_program();
    test_multiplication_program();
    test_bracket_validation();
    test_input_handling();
    test_memory_operations();
    test_reset_functionality();
    test_edge_cases();
    test_performance_large_program();
    test_hello_world_program();
    
    printf("\n==========================================\n");
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