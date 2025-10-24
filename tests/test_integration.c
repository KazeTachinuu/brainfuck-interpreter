#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../brainfuck.h"

// Helper function to read a file
char* read_file(const char* filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        return NULL;
    }
    
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char *content = malloc(length + 1);
    if (!content) {
        fclose(file);
        return NULL;
    }
    
    size_t bytes_read = fread(content, 1, length, file);
    (void)bytes_read; // Suppress unused variable warning
    content[length] = '\0';
    fclose(file);
    
    return content;
}

// Test that example programs can be loaded and executed without crashing
Test(integration, example_programs_load) {
    const char* example_files[] = {
        "example/ConwayGameOfLife.bf",
        "example/Exponential.bf", 
        "example/GoldenRatio.bf",
        "example/Sierpinski.bf",
        "example/TicTacToe.bf"
    };
    
    for (int i = 0; i < 5; i++) {
        char *content = read_file(example_files[i]);
        cr_assert_not_null(content, "Failed to read %s", example_files[i]);
        
        brainfuck_state_t state;
        brainfuck_init(&state);
        
        // Test that the program can be parsed and executed without crashing
        int result = brainfuck_interpret(&state, content);
        cr_assert_eq(result, 0, "Failed to execute %s", example_files[i]);
        
        free(content);
    }
}

// Test specific known behaviors of example programs
Test(integration, exponential_program) {
    char *content = read_file("example/Exponential.bf");
    cr_assert_not_null(content, "Failed to read Exponential.bf");
    
    brainfuck_state_t state;
    brainfuck_init(&state);
    
    // The exponential program should execute without errors
    int result = brainfuck_interpret(&state, content);
    cr_assert_eq(result, 0, "Exponential program failed to execute");
    
    free(content);
}

Test(integration, golden_ratio_program) {
    char *content = read_file("example/GoldenRatio.bf");
    cr_assert_not_null(content, "Failed to read GoldenRatio.bf");
    
    brainfuck_state_t state;
    brainfuck_init(&state);
    
    // The golden ratio program should execute without errors
    int result = brainfuck_interpret(&state, content);
    cr_assert_eq(result, 0, "Golden ratio program failed to execute");
    
    free(content);
}

Test(integration, sierpinski_program) {
    char *content = read_file("example/Sierpinski.bf");
    cr_assert_not_null(content, "Failed to read Sierpinski.bf");
    
    brainfuck_state_t state;
    brainfuck_init(&state);
    
    // The Sierpinski program should execute without errors
    int result = brainfuck_interpret(&state, content);
    cr_assert_eq(result, 0, "Sierpinski program failed to execute");
    
    free(content);
}

Test(integration, conway_game_of_life) {
    char *content = read_file("example/ConwayGameOfLife.bf");
    cr_assert_not_null(content, "Failed to read ConwayGameOfLife.bf");
    
    brainfuck_state_t state;
    brainfuck_init(&state);
    
    // The Conway's Game of Life program should execute without errors
    int result = brainfuck_interpret(&state, content);
    cr_assert_eq(result, 0, "Conway's Game of Life program failed to execute");
    
    free(content);
}

Test(integration, tic_tac_toe) {
    char *content = read_file("example/TicTacToe.bf");
    cr_assert_not_null(content, "Failed to read TicTacToe.bf");
    
    brainfuck_state_t state;
    brainfuck_init(&state);
    
    // The TicTacToe program should execute without errors
    int result = brainfuck_interpret(&state, content);
    cr_assert_eq(result, 0, "TicTacToe program failed to execute");
    
    free(content);
}

// Test that programs with input work correctly
Test(integration, input_handling) {
    brainfuck_state_t state;
    brainfuck_init(&state);
    
    // Test a simple program that reads input and outputs it
    const char* program = ",.";
    const char* input = "A";
    
    int result = brainfuck_interpret_with_input(&state, program, input);
    cr_assert_eq(result, 0, "Input handling failed");
    cr_assert_eq(state.array[0], 65, "Input not read correctly"); // ASCII 'A'
}

// Test error handling with malformed programs
Test(integration, error_handling) {
    brainfuck_state_t state;
    brainfuck_init(&state);
    
    // Test unmatched brackets
    int result = brainfuck_interpret(&state, "[");
    cr_assert_neq(result, 0, "Should fail on unmatched '['");
    
    result = brainfuck_interpret(&state, "]");
    cr_assert_neq(result, 0, "Should fail on unmatched ']'");
    
    result = brainfuck_interpret(&state, "[[");
    cr_assert_neq(result, 0, "Should fail on unmatched '['");
    
    result = brainfuck_interpret(&state, "]]");
    cr_assert_neq(result, 0, "Should fail on unmatched ']'");
}

// Test memory usage and limits
Test(integration, memory_limits) {
    brainfuck_state_t state;
    brainfuck_init(&state);
    
    // Test that we can use a reasonable amount of memory
    const char* program = ">+>+>+>+>+>+>+>+>+>+"; // Move right and increment 10 times
    int result = brainfuck_interpret(&state, program);
    cr_assert_eq(result, 0, "Memory usage test failed");
    
    // Verify memory values
    for (int i = 1; i <= 10; i++) {
        cr_assert_eq(state.array[i], 1, "Memory cell %d should be 1", i);
    }
    cr_assert_eq(state.memp, 10, "Memory pointer should be at position 10");
}

// Test performance with large programs
Test(integration, performance_large_program) {
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
    
    int result = brainfuck_interpret(&state, large_program);
    cr_assert_eq(result, 0, "Large program execution failed");
    cr_assert_eq(state.array[0], 50, "Final value should be 50");
}