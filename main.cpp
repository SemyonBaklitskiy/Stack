#include "functions.h"
#include <stdio.h>

#define stack_distor(pointer) if (stack_distructor(pointer) != NOERRORS) return -1
#define stack_ctor(pointer, capacity) if (stack_constructor(pointer, capacity, VARNAME(pointer),__FILE__, __PRETTY_FUNCTION__, __LINE__) != NOERRORS) return -1 
#define push(pointer, element) if (stack_push(pointer, element) != NOERRORS) return -1
#define pop(pointer, element) if (stack_pop(pointer, element) != NOERRORS) return -1

int main() {
    struct stack st;
    
    stack_ctor(&st, 2);

    elem_t element = {1, 2, 3};

    for(int i = 0; i < 5; ++i) {
        push(&st, element);
    }

    for(int i = 0; i < 5; ++i) {
        pop(&st, &element);
    }

    stack_distructor(&st);

    return 0;
}
