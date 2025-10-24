#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include "../brainfuck.h"

Test(brainfuck, minimal_test) {
    brainfuck_state_t state;
    brainfuck_init(&state);
    
    cr_assert_eq(state.stackp, 0);
    cr_assert_eq(state.memp, 0);
}