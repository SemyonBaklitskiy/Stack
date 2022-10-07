#ifdef INT
    typedef long int elem_t;
#else
    typedef double elem_t;
#endif

/// enum type errors
enum errors {
    NOERRORS = 0, ///< If no errors happened
    OVERFLOW = 1, ///< If there was stack overflow (realloc returned NULL in push)
    UNDERFLOW = 2, ///< If there was stack underflow (pop when size == 0) 
    MISMATCHSIZE = 3, ///< If there was mismatch between stack size and size in data.txt
    MISMATCHCAPACITY = 4, ///< If there was mismatch between stack capacity and capacity in data.txt
    MISMATCHELEMENT = 5, ///< If there was mismatch between stack element(s) and element(s) in data.txt
    MISMATCHPOINTERTOBUFFER = 6, ///< If there was mismatch between buffer pointer and buffer pointer in data.txt
    BUFFERISNULL = 7, ///< If realloc returned NULL in pop
    WRONGPARAMETERS = 8, ///< If wrong parameters were given to stack_constructor (stack_distructor)
    NULLPTR = 9, ///< If in any function (except c-tor, dis-tor, dump) were given NULL as a parameter
    FILEWASNTOPEN = 10, ///< If any file wasn`t open
};

struct stack {
    elem_t* buffer;
    unsigned int size;
    unsigned int capacity; 
    const char* file;
    const char* function;
    int line;
    const char* name;
};

void stack_constructor(struct stack* st, const int cp, const char* name, const char* file, const char* function, const int line);

void stack_distructor(struct stack* st);

elem_t pop(struct stack* st);

void push(struct stack* st, const elem_t element);

