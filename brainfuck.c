#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARRAYSIZE 16777216
#define MAXCODESIZE 65536

int stack[MAXCODESIZE], stackp;
char code[MAXCODESIZE];
int codelength;
short int array[ARRAYSIZE], memp;
int targets[MAXCODESIZE];
int c;
FILE *prog;

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

void handle_comma() {
    if ((c = getchar()) != EOF) {
        array[memp] = c == '\n' ? 10 : c;
    }
}

void handle_dot() {
    putchar(array[memp] == 10 ? '\n' : array[memp]);
    fflush(stdout);
}


int interpretBrainfuck(char *code) {
    for (size_t codep = 0; codep < strlen(code); codep++) {
        if (code[codep] == '[')
            stack[stackp++] = codep;
        if (code[codep] == ']') {
            if (stackp == 0) {
                fprintf(stderr, "Unmatched ']' at byte %ld.", codep);
                exit(1);
            } else {
                --stackp;
                targets[codep] = stack[stackp];
                targets[stack[stackp]] = codep;
            }
        }
    }
    if (stackp > 0) {
        fprintf(stderr, "Unmatched '[' at byte %d.", stack[--stackp]);
        exit(1);
    }

    for (size_t codep = 0; codep < strlen(code); codep++) {
        switch (code[codep]) {
            case '+': array[memp]++; break;
            case '-': array[memp]--; break;
            case '<': memp--; break;
            case '>': memp++; break;
            case ',': handle_comma(); break;
            case '.': handle_dot(); break;
            case '[': if (!array[memp]) codep = targets[codep]; break;
            case ']': if (array[memp]) codep = targets[codep]; break;
        }
    }
    return 0;
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
    if (argc == 3 && strcmp(option, "-c") == 0) {
        // Run Brainfuck code directly from the command line
        input = argv[2];
        codelength = strlen(input);
        strcpy(code, input);
    } else if (argc == 2) {
        // Default behavior: interpret Brainfuck code from the provided file
        input = argv[1];
        if (!(prog = fopen(input, "r"))) {
            fprintf(stderr, "Can't open the file %s.\n", input);
            exit(1);
        }
        codelength = fread(code, 1, MAXCODESIZE, prog);
        fclose(prog);
    } else {
        fprintf(stderr, "bf: Invalid arguments.\n");
        displaySeeHelp();
        exit(1);
    }

    interpretBrainfuck(code);

    return 0;
}

