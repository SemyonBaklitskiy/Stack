struct example {
    int id;
    const char* str;
};

#ifdef INT
    typedef int elem_t;

#elif defined DOUBLE
    typedef double elem_t;

#else 
    typedef example elem_t;
#endif

/// enum type errors
enum errors {
    NOERRORS = 0, ///< If no errors happened
    OVERFLOW = 1, ///< If there was stack overflow (realloc returned NULL in push)
    UNDERFLOW = 2, ///< If there was stack underflow (pop when size == 0) 
    MISMATCHSIZE = 3, ///< If there was mismatch between stack size and size in data.txt
    MISMATCHCAPACITY = 4, ///< If there was mismatch between stack capacity and capacity in data.txt
    MISMATCHELEMENT = 5, ///< If there was mismatch between stack element(s) and element(s) in data.txt (only for int and double)
    MISMATCHPOINTERTOBUFFER = 6, ///< If there was mismatch between buffer pointer and buffer pointer in data.txt
    BUFFERISNULL = 7, ///< If realloc returned NULL in pop
    WRONGPARAMETERS = 8, ///< If wrong parameters were given to stack_constructor (stack_distructor)
    NULLPTR = 9, ///< If in any function (except c-tor, dis-tor, dump) were given NULL as a parameter
    FILEWASNTOPEN = 10, ///< If any file wasn`t open
    MISMACHSTRUCTCANARY = 11, ///< If canary in structure was broken
    MISMATCHBUFFERCANARY = 12, ///< If canary in buffer was broken
};

struct stack {
#ifdef CANARY_PROT
    unsigned long long int leftCanary;
#endif
    elem_t* buffer;
    unsigned int size;
    unsigned int capacity; 
    const char* file;
    const char* function;
    int line;
    const char* name;
    errors error;
#ifdef CANARY_PROT
    unsigned long long rightCanary;
#endif
};

errors stack_constructor(struct stack* st, const int cp, const char* name, const char* file, const char* function, const int line);

errors stack_distructor(struct stack* st);

errors stack_pop(struct stack* st, elem_t* element);

errors stack_push(struct stack* st, const elem_t element);

#ifdef DEBUG
void debug(struct stack* st);
#endif

#define VARNAME(var) #var + (#var[0] == '&')

