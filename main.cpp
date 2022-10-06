#include "functions.h"
#define stack_ctor(name, capacity) stack_constructor(name, capacity, __FILE__, __PRETTY_FUNCTION__, __LINE__)

int main() {
    struct stack st;
    
    stack_ctor(&st, 2);

    push(&st, 11);
    push(&st, -2);
    pop(&st);

    stack_distructor(&st);
}
