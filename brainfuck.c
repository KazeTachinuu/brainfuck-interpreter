#include "brainfuck.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void brainfuck_init(brainfuck_state_t *state) {
    memset(state->stack, 0, sizeof(state->stack));
    state->stackp = 0;
    memset(state->code, 0, sizeof(state->code));
    state->codelength = 0;
    memset(state->array, 0, sizeof(state->array));
    state->memp = 0;
    memset(state->targets, 0, sizeof(state->targets));
    state->c = 0;
}

int brainfuck_validate_brackets(const char *code, size_t length) {
    int stack[MAXCODESIZE];
    int stackp = 0;
    
    for (size_t i = 0; i < length; i++) {
        if (code[i] == '[') {
            if (stackp >= MAXCODESIZE) {
                fprintf(stderr, "Too many nested loops (max %d)\n", MAXCODESIZE);
                return -1;
            }
            stack[stackp++] = i;
        } else if (code[i] == ']') {
            if (stackp == 0) {
                fprintf(stderr, "Unmatched ']' at byte %ld\n", i);
                return -1;
            }
            --stackp;
        }
    }
    
    if (stackp > 0) {
        fprintf(stderr, "Unmatched '[' at byte %d\n", stack[--stackp]);
        return -1;
    }
    
    return 0;
}

int brainfuck_interpret(brainfuck_state_t *state, const char *code) {
    return brainfuck_interpret_with_input(state, code, NULL);
}

int brainfuck_interpret_with_input(brainfuck_state_t *state, const char *code, const char *input) {
    size_t code_length = strlen(code);
    
    // Validate brackets first
    if (brainfuck_validate_brackets(code, code_length) != 0) {
        return -1;
    }
    
    // Reset state
    brainfuck_reset(state);
    
    // Build jump table for loops
    for (size_t codep = 0; codep < code_length; codep++) {
        if (code[codep] == '[') {
            state->stack[state->stackp++] = codep;
        } else if (code[codep] == ']') {
            if (state->stackp == 0) {
                fprintf(stderr, "Unmatched ']' at byte %ld\n", codep);
                return -1;
            } else {
                --state->stackp;
                state->targets[codep] = state->stack[state->stackp];
                state->targets[state->stack[state->stackp]] = codep;
            }
        }
    }
    
    if (state->stackp > 0) {
        fprintf(stderr, "Unmatched '[' at byte %d\n", state->stack[--state->stackp]);
        return -1;
    }
    
    // Set up input stream if provided
    FILE *input_stream = NULL;
    if (input != NULL) {
        input_stream = fmemopen((void*)input, strlen(input), "r");
        if (input_stream == NULL) {
            fprintf(stderr, "Failed to create input stream\n");
            return -1;
        }
    }
    
    // Execute the Brainfuck program
    for (size_t codep = 0; codep < code_length; codep++) {
        switch (code[codep]) {
            case '+': 
                state->array[state->memp]++; 
                break;
            case '-': 
                state->array[state->memp]--; 
                break;
            case '<': 
                if (state->memp > 0) state->memp--; 
                break;
            case '>': 
                if (state->memp < ARRAYSIZE - 1) state->memp++; 
                break;
            case ',': 
                if (input_stream != NULL) {
                    if ((state->c = fgetc(input_stream)) != EOF) {
                        state->array[state->memp] = state->c == '\n' ? 10 : state->c;
                    }
                } else {
                    if ((state->c = getchar()) != EOF) {
                        state->array[state->memp] = state->c == '\n' ? 10 : state->c;
                    }
                }
                break;
            case '.': 
                putchar(state->array[state->memp] == 10 ? '\n' : state->array[state->memp]); 
                fflush(stdout); 
                break;
            case '[': 
                if (!state->array[state->memp]) codep = state->targets[codep]; 
                break;
            case ']': 
                if (state->array[state->memp]) codep = state->targets[codep]; 
                break;
        }
    }
    
    if (input_stream != NULL) {
        fclose(input_stream);
    }
    
    return 0;
}

void brainfuck_reset(brainfuck_state_t *state) {
    memset(state->stack, 0, sizeof(state->stack));
    state->stackp = 0;
    memset(state->array, 0, sizeof(state->array));
    state->memp = 0;
    memset(state->targets, 0, sizeof(state->targets));
    state->c = 0;
}

// Helper functions for testing
const char* brainfuck_get_output(brainfuck_state_t *state) {
    // This is a placeholder - in a real implementation, you'd capture output
    // For now, we'll return NULL as output is printed directly to stdout
    (void)state; // Suppress unused parameter warning
    return NULL;
}

int brainfuck_get_memory_value(brainfuck_state_t *state, int position) {
    if (position >= 0 && position < ARRAYSIZE) {
        return state->array[position];
    }
    return -1;
}

int brainfuck_get_memory_pointer(brainfuck_state_t *state) {
    return state->memp;
}

// Original main function for backward compatibility
void displayHelp() {
    printf("Brainfuck Interpreter\n");
    printf("Usage: bf [-c code | filename]\n");
    printf("Options:\n");
    printf("  -c <code>     Run Brainfuck code directly from the command line\n");
    printf("  -h, --help    Display this help message\n");
}

void displaySeeHelp() {
    printf("Try 'bf --help' for more information.\n");
}

int main(int argc, char **argv) {
    if (argc == 1) {
        fprintf(stderr, "bf: No arguments provided.\n");
        displaySeeHelp();
        exit(1);
    }

    char *option = argv[1];

    if (argc == 2 && strcmp(option, "-c") == 0) {
        fprintf(stderr, "bf: Missing code\n");
        displaySeeHelp();
        exit(1);
    }
    if (strcmp(option, "-h") == 0 || strcmp(option, "--help") == 0){
        displayHelp();
        exit(0);
    }

    char *input;
    brainfuck_state_t state;
    brainfuck_init(&state);
    
    if (argc == 3 && strcmp(option, "-c") == 0) {
        // Run Brainfuck code directly from the command line
        input = argv[2];
        state.codelength = strlen(input);
        strcpy(state.code, input);
    } else if (argc == 2) {
        // Default behavior: interpret Brainfuck code from the provided file
        input = argv[1];
        FILE *prog;
        if (!(prog = fopen(input, "r"))) {
            fprintf(stderr, "Can't open the file %s.\n", input);
            exit(1);
        }
        state.codelength = fread(state.code, 1, MAXCODESIZE, prog);
        fclose(prog);
    } else {
        fprintf(stderr, "bf: Invalid arguments.\n");
        displaySeeHelp();
        exit(1);
    }

    int result = brainfuck_interpret(&state, state.code);
    return result;
}