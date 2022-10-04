/// enum type errors
enum errors {
    NOERRORS, ///< If no errors happened
    OVERFLOW, ///< If there was stack overflow (realloc returned NULL in push)
    UNDERFLOW, ///< If there was stack underflow (pop when size == 0) 
    MISMATCHSIZE, ///< If there was mismatch between stack size and size in data.txt
    MISMATCHCAPACITY, ///< If there was mismatch between stack capacity and capacity in data.txt
    MISMATCHELEMENT, ///< If there was mismatch between stack element(s) and element(s) in data.txt
    MISMATCHPOINTERTOBUFFER, ///< If there was mismatch between buffer pointer and buffer pointer in data.txt
    BUFFERISNULL, ///< If realloc returned NULL in pop
    WRONGPARAMETERS, ///< If wrong parameters were given to stack_constructor (stack_distructor)
    NULLPTR, ///< If in any function (except c-tor, dis-tor, dump) were given NULL as a parameter
    FILEWASNTOPEN, ///< If any file wasn`t open
    STACKDUMP ///< If wrong parameters were given to stack_dump 
};

struct stack {
    double* buffer;
    unsigned int size;
    unsigned int capacity; 
};

bool compare_two_numbers(double a, double b);

void stack_constructor(struct stack* stackName, unsigned int size, unsigned int capacity);

void stack_distructor(struct stack* stackName);

void verification(struct stack* stackName);

void resize(struct stack* stackName, unsigned int oldCapacity, unsigned int newCapacity);

void pop(struct stack* stackName, double* element);

void push(struct stack* stackName, double element);

void output_to_file(struct stack* stackName);

void stack_dump(struct stack* stackName, errors error, const char* file, const char* function, const int line);
