#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../brainfuck.h"

// Test various error conditions
Test(error_handling, invalid_brackets) {
    brainfuck_state_t state;
    brainfuck_init(&state);
    
    // Test various invalid bracket combinations
    const char* invalid_programs[] = {
        "[",           // Unmatched opening bracket
        "]",           // Unmatched closing bracket
        "][",          // Wrong order
        "[[",          // Two unmatched opening brackets
        "]]",          // Two unmatched closing brackets
        "[[]",         // Nested unmatched opening bracket
        "[]]",         // Nested unmatched closing bracket
        "[[[]]]]",     // Extra closing bracket
        "[[[[]]]",     // Extra opening bracket
        "[[][]]",      // Valid nested brackets
        "[[[]]]",      // Valid deeply nested brackets
    };
    
    int expected_results[] = {
        -1,  // [
        -1,  // ]
        -1,  // ][
        -1,  // [[
        -1,  // ]]
        -1,  // [[]
        -1,  // []]
        -1,  // [[[]]]]
        -1,  // [[[[]]]
        0,   // [[][]]
        0,   // [[[]]]
    };
    
    for (int i = 0; i < 11; i++) {
        brainfuck_reset(&state);
        int result = brainfuck_interpret(&state, invalid_programs[i]);
        cr_assert_eq(result, expected_results[i], 
                    "Program '%s' should have result %d, got %d", 
                    invalid_programs[i], expected_results[i], result);
    }
}

// Test memory boundary conditions
Test(error_handling, memory_boundaries) {
    brainfuck_state_t state;
    brainfuck_init(&state);
    
    // Test that we can't go below memory position 0
    cr_assert_eq(brainfuck_interpret(&state, "<"), 0);
    cr_assert_eq(state.memp, 0, "Memory pointer should not go below 0");
    
    // Test that we can't go above ARRAYSIZE-1
    // Move to the maximum position
    for (int i = 0; i < ARRAYSIZE - 1; i++) {
        cr_assert_eq(brainfuck_interpret(&state, ">"), 0);
    }
    cr_assert_eq(state.memp, ARRAYSIZE - 1, "Should be at maximum memory position");
    
    // Try to go beyond the limit
    cr_assert_eq(brainfuck_interpret(&state, ">"), 0);
    cr_assert_eq(state.memp, ARRAYSIZE - 1, "Should not go beyond maximum memory position");
}

// Test null pointer handling
Test(error_handling, null_pointers) {
    brainfuck_state_t state;
    brainfuck_init(&state);
    
    // Test with NULL code
    int result = brainfuck_interpret(&state, NULL);
    cr_assert_neq(result, 0, "Should handle NULL code gracefully");
    
    // Test with empty string
    result = brainfuck_interpret(&state, "");
    cr_assert_eq(result, 0, "Empty string should be valid");
}

// Test very long programs
Test(error_handling, very_long_programs) {
    brainfuck_state_t state;
    brainfuck_init(&state);
    
    // Create a program that's exactly MAXCODESIZE
    char long_program[MAXCODESIZE + 1];
    for (int i = 0; i < MAXCODESIZE; i++) {
        long_program[i] = '+';
    }
    long_program[MAXCODESIZE] = '\0';
    
    // This should work
    int result = brainfuck_interpret(&state, long_program);
    cr_assert_eq(result, 0, "Long program should execute successfully");
    cr_assert_eq(state.array[0], MAXCODESIZE, "Should have incremented MAXCODESIZE times");
}

// Test deeply nested loops
Test(error_handling, deeply_nested_loops) {
    brainfuck_state_t state;
    brainfuck_init(&state);
    
    // Create a program with many nested loops
    char nested_program[1000];
    int pos = 0;
    
    // Create 10 levels of nesting
    for (int i = 0; i < 10; i++) {
        nested_program[pos++] = '[';
        nested_program[pos++] = '+';
    }
    
    // Close all the loops
    for (int i = 0; i < 10; i++) {
        nested_program[pos++] = ']';
    }
    
    nested_program[pos] = '\0';
    
    int result = brainfuck_interpret(&state, nested_program);
    cr_assert_eq(result, 0, "Deeply nested loops should work");
}

// Test infinite loops (with timeout protection)
Test(error_handling, infinite_loops) {
    brainfuck_state_t state;
    brainfuck_init(&state);
    
    // Test a simple infinite loop
    // Note: In a real implementation, you might want to add timeout protection
    // For now, we'll just test that it doesn't crash immediately
    // const char* infinite_loop = "+[]"; // Set cell to 1, then infinite loop
    
    // This test might hang in a real scenario, but we'll limit it
    // by using a small test that should complete quickly
    const char* finite_loop = "+[-]"; // Set cell to 1, then loop until 0
    int result = brainfuck_interpret(&state, finite_loop);
    cr_assert_eq(result, 0, "Finite loop should complete");
    cr_assert_eq(state.array[0], 0, "Cell should be 0 after finite loop");
}

// Test input handling edge cases
Test(error_handling, input_edge_cases) {
    brainfuck_state_t state;
    brainfuck_init(&state);
    
    // Test with empty input
    int result = brainfuck_interpret_with_input(&state, ",", "");
    cr_assert_eq(result, 0, "Empty input should be handled gracefully");
    
    // Test with NULL input
    result = brainfuck_interpret_with_input(&state, ",", NULL);
    cr_assert_eq(result, 0, "NULL input should be handled gracefully");
    
    // Test with very long input
    char long_input[1000];
    for (int i = 0; i < 999; i++) {
        long_input[i] = 'A';
    }
    long_input[999] = '\0';
    
    result = brainfuck_interpret_with_input(&state, ",", long_input);
    cr_assert_eq(result, 0, "Long input should be handled");
    cr_assert_eq(state.array[0], 65, "Should read first character of long input");
}

// Test memory overflow protection
Test(error_handling, memory_overflow_protection) {
    brainfuck_state_t state;
    brainfuck_init(&state);
    
    // Test that we can't cause integer overflow in memory cells
    // This is more of a stress test
    const char* overflow_test = "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++";
    
    int result = brainfuck_interpret(&state, overflow_test);
    cr_assert_eq(result, 0, "Overflow test should complete");
    
    // The exact value depends on the data type used for array elements
    // We just want to ensure it doesn't crash
    cr_assert_gt(state.array[0], 0, "Cell should have a positive value");
}

// Test stack overflow protection
Test(error_handling, stack_overflow_protection) {
    brainfuck_state_t state;
    brainfuck_init(&state);
    
    // Create a program with many opening brackets
    char stack_overflow_program[MAXCODESIZE + 1];
    int pos = 0;
    
    // Add MAXCODESIZE opening brackets
    for (int i = 0; i < MAXCODESIZE; i++) {
        stack_overflow_program[pos++] = '[';
    }
    
    stack_overflow_program[pos] = '\0';
    
    // This should fail due to too many nested loops
    int result = brainfuck_interpret(&state, stack_overflow_program);
    cr_assert_neq(result, 0, "Should fail on too many nested loops");
}