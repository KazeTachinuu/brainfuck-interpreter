#include "brainfuck.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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