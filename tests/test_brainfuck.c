#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../brainfuck.h"

// Test fixture for setting up a clean brainfuck state
void setup_brainfuck_state(void) {
    // This will be called before each test
}

void teardown_brainfuck_state(void) {
    // This will be called after each test
}

// Test basic initialization
Test(brainfuck, initialization) {
    brainfuck_state_t state;
    brainfuck_init(&state);
    
    cr_assert_eq(state.stackp, 0);
    cr_assert_eq(state.memp, 0);
    cr_assert_eq(state.c, 0);
    cr_assert_eq(state.codelength, 0);
    
    // Check that memory is zeroed
    for (int i = 0; i < 10; i++) {
        cr_assert_eq(state.array[i], 0);
    }
}

// Test bracket validation
Test(brainfuck, bracket_validation_valid) {
    cr_assert_eq(brainfuck_validate_brackets("[]", 2), 0);
    cr_assert_eq(brainfuck_validate_brackets("[[]]", 4), 0);
    cr_assert_eq(brainfuck_validate_brackets("[[[]]]", 6), 0);
    cr_assert_eq(brainfuck_validate_brackets("+++[>+++<-]", 11), 0);
    cr_assert_eq(brainfuck_validate_brackets("", 0), 0);
    cr_assert_eq(brainfuck_validate_brackets("++++", 4), 0);
}

Test(brainfuck, bracket_validation_invalid) {
    cr_assert_neq(brainfuck_validate_brackets("]", 1), 0);
    cr_assert_neq(brainfuck_validate_brackets("[", 1), 0);
    cr_assert_neq(brainfuck_validate_brackets("][", 2), 0);
    cr_assert_neq(brainfuck_validate_brackets("[[", 2), 0);
    cr_assert_neq(brainfuck_validate_brackets("]]", 2), 0);
    cr_assert_neq(brainfuck_validate_brackets("[[]", 3), 0);
}

// Test increment operation
Test(brainfuck, increment_operation) {
    brainfuck_state_t state;
    brainfuck_init(&state);
    
    // Test single increment
    cr_assert_eq(brainfuck_interpret(&state, "+"), 0);
    cr_assert_eq(state.array[0], 1);
    
    // Test multiple increments
    brainfuck_reset(&state);
    cr_assert_eq(brainfuck_interpret(&state, "+++"), 0);
    cr_assert_eq(state.array[0], 3);
    
    // Test increment at different memory positions
    brainfuck_reset(&state);
    cr_assert_eq(brainfuck_interpret(&state, ">+"), 0);
    cr_assert_eq(state.array[1], 1);
    cr_assert_eq(state.memp, 1);
}

// Test decrement operation
Test(brainfuck, decrement_operation) {
    brainfuck_state_t state;
    brainfuck_init(&state);
    
    // Test single decrement (should wrap around to 255)
    cr_assert_eq(brainfuck_interpret(&state, "-"), 0);
    cr_assert_eq(state.array[0], -1);
    
    // Test decrement after increment
    brainfuck_reset(&state);
    cr_assert_eq(brainfuck_interpret(&state, "++-"), 0);
    cr_assert_eq(state.array[0], 1);
    
    // Test multiple decrements
    brainfuck_reset(&state);
    cr_assert_eq(brainfuck_interpret(&state, "+++---"), 0);
    cr_assert_eq(state.array[0], 0);
}

// Test pointer movement
Test(brainfuck, pointer_movement) {
    brainfuck_state_t state;
    brainfuck_init(&state);
    
    // Test right movement
    cr_assert_eq(brainfuck_interpret(&state, ">"), 0);
    cr_assert_eq(state.memp, 1);
    
    // Test left movement
    brainfuck_reset(&state);
    cr_assert_eq(brainfuck_interpret(&state, "><"), 0);
    cr_assert_eq(state.memp, 0);
    
    // Test multiple movements
    brainfuck_reset(&state);
    cr_assert_eq(brainfuck_interpret(&state, ">>>"), 0);
    cr_assert_eq(state.memp, 3);
    
    // Test movement with operations
    brainfuck_reset(&state);
    cr_assert_eq(brainfuck_interpret(&state, ">+<+"), 0);
    cr_assert_eq(state.array[0], 1);
    cr_assert_eq(state.array[1], 1);
    cr_assert_eq(state.memp, 0);
}

// Test simple loops
Test(brainfuck, simple_loops) {
    brainfuck_state_t state;
    brainfuck_init(&state);
    
    // Test loop that doesn't execute (cell is 0)
    cr_assert_eq(brainfuck_interpret(&state, "[+]"), 0);
    cr_assert_eq(state.array[0], 0);
    
    // Test loop that executes once
    brainfuck_reset(&state);
    cr_assert_eq(brainfuck_interpret(&state, "+[-]"), 0);
    cr_assert_eq(state.array[0], 0);
    
    // Test loop that executes multiple times
    brainfuck_reset(&state);
    cr_assert_eq(brainfuck_interpret(&state, "+++[-]"), 0);
    cr_assert_eq(state.array[0], 0);
}

// Test complex loops
Test(brainfuck, complex_loops) {
    brainfuck_state_t state;
    brainfuck_init(&state);
    
    // Test nested loops
    cr_assert_eq(brainfuck_interpret(&state, "+++[>+[>+<-]<-]"), 0);
    cr_assert_eq(state.array[0], 0);
    cr_assert_eq(state.array[1], 0);
    cr_assert_eq(state.array[2], 3);
    
    // Test loop with pointer movement
    brainfuck_reset(&state);
    cr_assert_eq(brainfuck_interpret(&state, "+++[>+<-]"), 0);
    cr_assert_eq(state.array[0], 0);
    cr_assert_eq(state.array[1], 3);
}

// Test output operation (basic test)
Test(brainfuck, output_operation) {
    brainfuck_state_t state;
    brainfuck_init(&state);
    
    // Test output of ASCII 'A' (65)
    cr_assert_eq(brainfuck_interpret(&state, "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++."), 0);
    // Note: We can't easily test stdout output in unit tests without capturing it
    // This test mainly ensures the operation doesn't crash
}

// Test input operation (basic test)
Test(brainfuck, input_operation) {
    brainfuck_state_t state;
    brainfuck_init(&state);
    
    // Test input with predefined input
    cr_assert_eq(brainfuck_interpret_with_input(&state, ",", "A"), 0);
    cr_assert_eq(state.array[0], 65); // ASCII 'A'
}

// Test memory boundaries
Test(brainfuck, memory_boundaries) {
    brainfuck_state_t state;
    brainfuck_init(&state);
    
    // Test that we can't go below 0
    cr_assert_eq(brainfuck_interpret(&state, "<"), 0);
    cr_assert_eq(state.memp, 0);
    
    // Test that we can't go above ARRAYSIZE-1
    // Move to the end and try to go further
    for (int i = 0; i < ARRAYSIZE; i++) {
        state.memp = i;
        cr_assert_eq(brainfuck_interpret(&state, ">"), 0);
        if (i < ARRAYSIZE - 1) {
            cr_assert_eq(state.memp, i + 1);
        } else {
            cr_assert_eq(state.memp, ARRAYSIZE - 1);
        }
    }
}

// Test reset functionality
Test(brainfuck, reset_functionality) {
    brainfuck_state_t state;
    brainfuck_init(&state);
    
    // Set some values
    cr_assert_eq(brainfuck_interpret(&state, "+++>>+"), 0);
    cr_assert_eq(state.array[0], 3);
    cr_assert_eq(state.array[2], 1);
    cr_assert_eq(state.memp, 2);
    
    // Reset and verify
    brainfuck_reset(&state);
    cr_assert_eq(state.memp, 0);
    cr_assert_eq(state.array[0], 0);
    cr_assert_eq(state.array[2], 0);
}

// Test helper functions
Test(brainfuck, helper_functions) {
    brainfuck_state_t state;
    brainfuck_init(&state);
    
    // Test memory value getter
    cr_assert_eq(brainfuck_get_memory_value(&state, 0), 0);
    cr_assert_eq(brainfuck_get_memory_value(&state, -1), -1);
    cr_assert_eq(brainfuck_get_memory_value(&state, ARRAYSIZE), -1);
    
    // Test memory pointer getter
    cr_assert_eq(brainfuck_get_memory_pointer(&state), 0);
    
    // Set some values and test
    cr_assert_eq(brainfuck_interpret(&state, ">+++"), 0);
    cr_assert_eq(brainfuck_get_memory_value(&state, 1), 3);
    cr_assert_eq(brainfuck_get_memory_pointer(&state), 1);
}

// Test edge cases
Test(brainfuck, edge_cases) {
    brainfuck_state_t state;
    brainfuck_init(&state);
    
    // Test empty program
    cr_assert_eq(brainfuck_interpret(&state, ""), 0);
    
    // Test program with only non-command characters
    cr_assert_eq(brainfuck_interpret(&state, "abc123!@#"), 0);
    
    // Test program with mixed commands and non-commands
    cr_assert_eq(brainfuck_interpret(&state, "a+b-c<d>e,f.g[h]i"), 0);
    cr_assert_eq(state.array[0], 0); // Should be 0 after +-
    cr_assert_eq(state.memp, 0); // Should be 0 after < >
}

// Test complex programs
Test(brainfuck, complex_programs) {
    brainfuck_state_t state;
    brainfuck_init(&state);
    
    // Test a program that sets cell 0 to 5
    cr_assert_eq(brainfuck_interpret(&state, "+++++"), 0);
    cr_assert_eq(state.array[0], 5);
    
    // Test a program that copies value from cell 0 to cell 1
    brainfuck_reset(&state);
    cr_assert_eq(brainfuck_interpret(&state, "+++++[>+<-]"), 0);
    cr_assert_eq(state.array[0], 0);
    cr_assert_eq(state.array[1], 5);
    
    // Test a program that adds two numbers (3 + 2 = 5)
    brainfuck_reset(&state);
    cr_assert_eq(brainfuck_interpret(&state, "+++>++<[>+<-]"), 0);
    cr_assert_eq(state.array[0], 0);
    cr_assert_eq(state.array[1], 5);
}