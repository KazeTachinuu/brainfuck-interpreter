#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEM_SIZE 30000
#define MAX_DEPTH 4096

static char *read_file(const char *path)
{
    FILE *f = fopen(path, "rb");
    if (!f)
        return NULL;

    fseek(f, 0, SEEK_END);
    long n = ftell(f);
    rewind(f);

    if (n < 0) {
        fclose(f);
        return NULL;
    }

    char *s = malloc((size_t)n + 1);
    if (!s) {
        fclose(f);
        return NULL;
    }
    size_t bytes_read = fread(s, 1, (size_t)n, f);
    int read_error = ferror(f);
    fclose(f);
    if (read_error) {
        free(s);
        return NULL;
    }
    s[bytes_read] = '\0';
    return s;
}

static int build_jumps(const char *code, size_t *jump, size_t len)
{
    size_t stack[MAX_DEPTH], depth = 0;

    for (size_t i = 0; i < len; i++) {
        if (code[i] == '[') {
            if (depth >= MAX_DEPTH)
                return -1;
            stack[depth++] = i;
        } else if (code[i] == ']') {
            if (!depth) {
                fprintf(stderr, "Error: Unmatched ']'\n");
                return -1;
            }
            size_t j = stack[--depth];
            jump[j] = i;
            jump[i] = j;
        }
    }
    if (depth) {
        fprintf(stderr, "Error: Unmatched '['\n");
        return -1;
    }
    return 0;
}

static int run(const char *code)
{
    size_t len = strlen(code);
    unsigned char *mem = calloc(MEM_SIZE, 1);
    size_t *jump = calloc(len, sizeof *jump);
    int rc = -1;

    if (!mem || !jump || build_jumps(code, jump, len) < 0)
        goto out;

    for (size_t i = 0, p = 0; i < len; i++) {
        switch (code[i]) {
        case '>': p = (p + 1) % MEM_SIZE; break;
        case '<': p = (p + MEM_SIZE - 1) % MEM_SIZE; break;
        case '+': mem[p]++; break;
        case '-': mem[p]--; break;
        case '.': putchar(mem[p]); break;
        case ',': { int c = getchar(); if (c != EOF) mem[p] = (unsigned char)c; } break;
        case '[': if (!mem[p]) i = jump[i]; break;
        case ']': if (mem[p]) i = jump[i]; break;
        }
    }
    rc = 0;
out:
    free(mem);
    free(jump);
    return rc;
}

int main(int argc, char **argv)
{
    if (argc < 2 || !strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
        printf("Usage: bf [-c code | file]\n");
        return argc < 2;
    }

    char *code = NULL;

    if (!strcmp(argv[1], "-c")) {
        if (argc > 2) {
            code = malloc(strlen(argv[2]) + 1);
            if (code)
                strcpy(code, argv[2]);
        }
    } else {
        code = read_file(argv[1]);
    }

    if (!code) {
        fprintf(stderr, "Error: Cannot load program\n");
        return 1;
    }

    int rc = run(code);
    free(code);
    return rc ? 1 : 0;
}
