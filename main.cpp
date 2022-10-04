#include "functions.h"

int main() {
    struct stack st;
    stack_constructor(&st, 0, 2);

    verification(&st);
    push(&st, 1.2);
    verification(&st);

    verification(&st);
    push(&st, 5.1);
    verification(&st);

    verification(&st);
    push(&st, -3.14);
    verification(&st);

    double element = 0;

    verification(&st);
    pop(&st, &element);
    verification(&st);

    verification(&st);
    pop(&st, &element);
    verification(&st);

}
