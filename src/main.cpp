#include "../includes/functions.h"
#include <stdio.h>

#ifdef DEBUG

#define stack_distor(pointer) if (stack_distructor(pointer) != NO_IMPORTANT_ERRORS) return -1; debug(pointer)
#define stack_ctor(pointer, capacity) if (stack_constructor(pointer, capacity, VARNAME(pointer),__FILE__, __PRETTY_FUNCTION__, __LINE__) != NO_IMPORTANT_ERRORS) return -1; debug(pointer) 
#define push(pointer, element) if (stack_push(pointer, element) != NO_IMPORTANT_ERRORS) return -1; debug(pointer)
#define pop(pointer, element) if (stack_pop(pointer, element) != NO_IMPORTANT_ERRORS) return -1; debug(pointer)

#else 

#define stack_distor(pointer) if (stack_distructor(pointer) != NO_IMPORTANT_ERRORS) return -1
#define stack_ctor(pointer, capacity) if (stack_constructor(pointer, capacity, VARNAME(pointer),__FILE__, __PRETTY_FUNCTION__, __LINE__) != NO_IMPORTANT_ERRORS) return -1
#define push(pointer, element) if (stack_push(pointer, element) != NO_IMPORTANT_ERRORS) return -1
#define pop(pointer, element) if (stack_pop(pointer, element) != NO_IMPORTANT_ERRORS) return -1

#endif

int main() {
    struct stack st;
    
    stack_ctor(&st, 1);

    elem_t element = 0;

    for(int i = 0; i < 5; ++i) {
        element = i;
        push(&st, element);
    }

    for(int i = 0; i < 5; ++i) {
        pop(&st, &element);
        printf("%d\n", element);
    }

    stack_distor(&st);

    return 0;
}
