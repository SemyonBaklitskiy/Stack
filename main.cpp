#include "functions.h"

#define VARNAME(var) #var + (#var[0] == '&')
#define stack_ctor(pointer, capacity) stack_constructor(pointer, capacity, VARNAME(pointer),__FILE__, __PRETTY_FUNCTION__, __LINE__)

int main() {
    struct stack st;
    
    stack_ctor(&st, 2);

    push(&st, 11);
    push(&st, -2);
    pop(&st);

    stack_distructor(&st);
}
