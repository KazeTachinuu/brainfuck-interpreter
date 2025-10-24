#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARRAYSIZE 16777216
#define MAXCODESIZE 65536

typedef struct {
    int stack[MAXCODESIZE];
    int stackp;
    char code[MAXCODESIZE];
    int codelength;
    short int *array;
    int memp;
    int targets[MAXCODESIZE];
    int c;
} brainfuck_state_t;

void brainfuck_init(brainfuck_state_t *state) {
    printf("Initializing brainfuck state...\n");
    memset(state->stack, 0, sizeof(state->stack));
    state->stackp = 0;
    memset(state->code, 0, sizeof(state->code));
    state->codelength = 0;
    
    // Allocate memory for array
    state->array = calloc(ARRAYSIZE, sizeof(short int));
    if (state->array == NULL) {
        printf("Failed to allocate memory for array\n");
        exit(1);
    }
    
    state->memp = 0;
    memset(state->targets, 0, sizeof(state->targets));
    state->c = 0;
    printf("Initialization complete.\n");
}

int main() {
    printf("Starting debug test...\n");
    
    brainfuck_state_t state;
    printf("Created state structure.\n");
    
    brainfuck_init(&state);
    printf("Called brainfuck_init.\n");
    
    printf("stackp: %d\n", state.stackp);
    printf("memp: %d\n", state.memp);
    printf("c: %d\n", state.c);
    
    printf("Debug test completed successfully.\n");
    return 0;
}