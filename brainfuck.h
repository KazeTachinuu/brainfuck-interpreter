#ifndef BRAINFUCK_H
#define BRAINFUCK_H

#include <stddef.h>
#include <stdio.h>

// Configuration constants
#define ARRAYSIZE 16777216
#define MAXCODESIZE 65536

// Brainfuck interpreter state structure
typedef struct {
    int stack[MAXCODESIZE];
    int stackp;
    char code[MAXCODESIZE];
    int codelength;
    short int array[ARRAYSIZE];
    int memp;
    int targets[MAXCODESIZE];
    int c;
} brainfuck_state_t;

// Function declarations
void brainfuck_init(brainfuck_state_t *state);
int brainfuck_validate_brackets(const char *code, size_t length);
int brainfuck_interpret(brainfuck_state_t *state, const char *code);
int brainfuck_interpret_with_input(brainfuck_state_t *state, const char *code, const char *input);
void brainfuck_reset(brainfuck_state_t *state);

// Helper functions for testing
const char* brainfuck_get_output(brainfuck_state_t *state);
int brainfuck_get_memory_value(brainfuck_state_t *state, int position);
int brainfuck_get_memory_pointer(brainfuck_state_t *state);

#endif // BRAINFUCK_H