struct example {
    int id;
    const char* str;
};

#define HASH_PROT

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
    MISMATCHHASH = 13, ///< If there was mismatch between current hash and st.hash field
};

struct stack {
#ifdef CANARY_PROT
    unsigned long long int leftCanary;
#endif
#ifdef HASH_PROT
    unsigned int hash;
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

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#define my_assert(error) processor_of_errors(error, __FILE__, __PRETTY_FUNCTION__, __LINE__)
#define dump(error) stack_dump(st, error, __FILE__, __PRETTY_FUNCTION__, __LINE__)

#ifdef INT
    #define FORMAT "%d\n"

#ifdef CANARY_PROT
    #define TEXT "data[%ld] = %d\n"
#endif
    
#ifndef CANARY_PROT
    #define TEXT "data[%d] = %d\n"
#endif
    const unsigned int poison = 0XDEADDA;

#elif defined DOUBLE
    const long unsigned int poison = 0XDEADDEAD;
    #define FORMAT "%lf\n"

#ifdef CANARY_PROT
    #define TEXT "data[%ld] = %lf\n"
#endif
    
#ifndef CANARY_PROT
    #define TEXT "data[%d] = %lf\n"
#endif

#else    
    const long unsigned int poison = 0XDEADDEAD;
#endif

const char* logFile = "output_files/log.txt";
const char* dataFile = "output_files/data.txt";

#ifdef DEBUG
const char* debugFile = "output_files/debug.txt";
#endif

#ifdef CANARY_PROT
    const unsigned long long int canaryDefinition = 0XBAADF00D;
#endif

#ifdef HASH_PROT
static unsigned int HashRot13(elem_t* st, const unsigned int bytes) {
    unsigned int hash = 0;

    for(unsigned int i = 0; i < bytes; ++i)
    {
        hash += (unsigned char)(*((char*)st + i));
        hash -= (hash << 13) | (hash >> 19);
    }

    return hash;
}
#endif

static void processor_of_errors(errors error, const char* function, const char* name, const int line) { 
    switch (error) {

    case FILEWASNTOPEN:
        printf("In file %s function %s on line %d: file wasn`t open\n", function, name, line);
        break;

    case NULLPTR:
        printf("In file %s function %s on line %d: NULL was given as a parameter\n", function, name, line);
        break;

    default:
        break;
    }   
}

static bool file_is_open(FILE* stream) {
    if (stream == NULL) 
        return false;

    return true;   
}

static void info_in_logfile(struct stack* st, FILE* stream) {
    if (st == NULL || stream == NULL) {
        my_assert(NULLPTR);
        return;
    }

    fprintf(stream, "Stack \"%s\" created in file %s in function %s on line %d\n", st->name, st->file, st->function, st->line);
    fprintf(stream, "Size = %d\n", st->size);
    fprintf(stream, "Capacity = %d\n", st->capacity);
    fprintf(stream, "Pointer to buffer = %p\n", st->buffer);
#ifdef HASH_PROT
    fprintf(stream, "Hash = %d\n", st->hash);
#endif
    fprintf(stream, "Error = %d\n", st->error);

#ifdef CANARY_PROT
    fprintf(stream, "Left canary in structure = %llX\n", st->leftCanary);
    fprintf(stream, "Right canary in structure = %llX\n", st->rightCanary);

    fprintf(stream, "Left canary in buffer = %llX\n", *((unsigned long long int*)st->buffer)); 
    fprintf(stream, "Right canary in buffer = %llX\n", *((unsigned long long int*)((char*)st->buffer + sizeof(canaryDefinition) + sizeof(elem_t) * st->capacity)));

#if defined INT || defined DOUBLE
    if (st->buffer == NULL) 
        return;

    for (unsigned int i = sizeof(canaryDefinition) / sizeof(elem_t); i < st->capacity + sizeof(canaryDefinition) / sizeof(elem_t); ++i) 
        fprintf(stream, TEXT, i - sizeof(canaryDefinition) / sizeof(elem_t), st->buffer[i]);
    
#endif

#endif

#ifndef CANARY_PROT

#if defined INT || defined DOUBLE
    if (st->buffer == NULL) 
        return;

    for (unsigned int i = 0; i < st->capacity; ++i) 
        fprintf(stream, TEXT, i, st->buffer[i]);
    
#endif

#endif
    
}

static void stack_dump(struct stack* st, errors error, const char* file, const char* function, const int line) {
    if (st == NULL || file == NULL || function == NULL) {
        my_assert(NULLPTR);
        return;
    }

    st->error = error;

    FILE* stream = fopen(logFile, "w");

    if (!file_is_open(stream)) {
        my_assert(FILEWASNTOPEN);
        return;
    }

    fprintf(stream, "In file %s in function %s on line %d error %d was happened.\nLook also at output_files/data.txt\n", file, function, line, error);
    info_in_logfile(st, stream);

    fclose(stream);
    printf("Look at the output_files/log.txt\n");   
}

static void output_to_file(struct stack* st) {
    if (st == NULL) {
        my_assert(NULLPTR);
        return;
    }

    FILE* file = fopen(dataFile, "w");

    if (!file_is_open(file)) {
        my_assert(FILEWASNTOPEN); 
        return;
    }

    unsigned int capacity = st->capacity;
    unsigned int size = st->size;
    elem_t* buffer = st->buffer;

    fprintf(file, "%d\n%d\n%p\n", size, capacity, buffer);

#if defined INT || defined DOUBLE

#ifdef CANARY_PROT
    if (buffer != NULL) {
        for (unsigned int i = sizeof(canaryDefinition) / sizeof(elem_t); i < capacity + sizeof(canaryDefinition) / sizeof(elem_t); ++i) 
            fprintf(file, FORMAT, buffer[i]);
    }

#endif

#ifndef CANARY_PROT
    if (buffer != NULL) {
        for (unsigned int i = 0; i < capacity; ++i) 
            fprintf(file, FORMAT, buffer[i]);

    }
#endif

#endif

    fclose(file);
}

#if defined INT || defined DOUBLE

static bool compare_two_numbers(elem_t a, elem_t b) {
#ifdef INT
    return a == b;

#else
    elem_t epsilon = 0.00001;

    if (fabs(a - b) <= epsilon) 
        return true;
    
    return false;
#endif
}

#endif

static errors verification(struct stack* st) {
    if (st == NULL) {
        my_assert(NULLPTR);
        return NULLPTR;
    }

    FILE* file = fopen(dataFile, "r");

    if (!file_is_open(file)) {
        my_assert(FILEWASNTOPEN);
        return FILEWASNTOPEN;
    }

#ifdef CANARY_PROT
    unsigned long long int leftCanary = st->leftCanary;
    unsigned long long int rightCanary = st->rightCanary;

    if ((leftCanary != canaryDefinition) || (rightCanary != canaryDefinition)) {
        dump(MISMACHSTRUCTCANARY);
        return MISMACHSTRUCTCANARY;
    }
    
    if (((*((unsigned long long int*)st->buffer)) != canaryDefinition) || 
        ((*((unsigned long long int*)((char*)st->buffer + sizeof(canaryDefinition) + st->capacity * sizeof(elem_t)))) != canaryDefinition)) {
            dump(MISMATCHBUFFERCANARY);
            return MISMATCHBUFFERCANARY;
     }
            
#endif

#ifdef HASH_PROT
    unsigned int currentHash = HashRot13(st->buffer, st->capacity * sizeof(elem_t));
    if (currentHash != st->hash) {
        dump(MISMATCHHASH);
        return MISMATCHHASH;
    }
#endif

    unsigned int capacity = st->capacity;
    unsigned int size = st->size;
    elem_t* buffer = st->buffer;

    unsigned int dataCapacity = 0;
    unsigned int dataSize = 0;
    elem_t* dataBuffer = NULL;

    fscanf(file, "%d\n%d\n%p\n", &dataSize, &dataCapacity, &dataBuffer);

    if (dataSize != size) {
        fclose(file);
        dump(MISMATCHSIZE);
        return MISMATCHSIZE;
    } 
     
    if (dataCapacity != capacity) {
        fclose(file);
        dump(MISMATCHCAPACITY);
        return MISMATCHCAPACITY;
    }   

    if (dataBuffer != buffer) {
        fclose(file);
        dump(MISMATCHPOINTERTOBUFFER);
        return MISMATCHPOINTERTOBUFFER;
    } 

#if defined INT || defined DOUBLE

#ifdef CANARY_PROT
    if ((dataBuffer != NULL) && (buffer != NULL)) {
        for (unsigned int i = sizeof(canaryDefinition) / sizeof(elem_t); i < capacity + sizeof(canaryDefinition) / sizeof(elem_t); ++i) {
            elem_t dataElement = poison;

            fscanf(file, FORMAT, &dataElement);

            if (!compare_two_numbers(dataElement, st->buffer[i])) {
                fclose(file);
                dump(MISMATCHELEMENT);
                return MISMATCHELEMENT;
            }
        }
    }

#endif

#ifndef CANARY_PROT

    if ((dataBuffer != NULL) && (buffer != NULL)) {
        for (unsigned int i = 0; i < capacity; ++i) {
            elem_t dataElement = poison;

            fscanf(file, FORMAT, &dataElement);

            if (!compare_two_numbers(dataElement, st->buffer[i])) {
                fclose(file);
                dump(MISMATCHELEMENT);
                return MISMATCHELEMENT;
            }
        }
    }
#endif

#endif

    fclose(file);
    return NOERRORS;
}

static void resize(struct stack* st, const unsigned int oldCapacity, const unsigned int newCapacity) {
    if (st == NULL){
        my_assert(NULLPTR);
        return;
    }

#ifdef CANARY_PROT
    st->buffer = (elem_t*)realloc(st->buffer, newCapacity * sizeof(elem_t) + 2 * sizeof(canaryDefinition));    
    
    if (st->buffer != NULL) {
        *((unsigned long long int*)st->buffer) = canaryDefinition; 
        *((unsigned long long int*)((char*)st->buffer + sizeof(canaryDefinition) + newCapacity * sizeof(elem_t))) = canaryDefinition;

        if (newCapacity > oldCapacity)
            memset((char*)st->buffer + sizeof(canaryDefinition) + oldCapacity * sizeof(elem_t), poison, (newCapacity - oldCapacity) * sizeof(elem_t));

    }
#endif

#ifndef CANARY_PROT
    if (st->buffer != NULL) {
        st->buffer = (elem_t*)realloc(st->buffer, newCapacity * sizeof(elem_t));

        if (newCapacity > oldCapacity)
            memset((char*)st->buffer + oldCapacity * sizeof(elem_t), poison, (newCapacity - oldCapacity) * sizeof(elem_t));
    }

#endif 
}

errors stack_constructor(struct stack* st, const int cp, const char* name, const char* file, const char* function, const int line) {
    if ((st == NULL) || (cp < 0) || (file == NULL) || (function == NULL) || (name == NULL)) {
        dump(WRONGPARAMETERS);
        return WRONGPARAMETERS;
    }

    unsigned int capacity = (unsigned int)cp;

    if (capacity == 0)
        capacity = 1;

#ifdef CANARY_PROT
    st->rightCanary = canaryDefinition;
    st->leftCanary = canaryDefinition;

    st->buffer = (elem_t*)calloc(capacity * sizeof(elem_t) + 2 * (sizeof(canaryDefinition)), sizeof(char));   

    *((unsigned long long int*)st->buffer) = canaryDefinition; 
    *((unsigned long long int*)((char*)st->buffer + sizeof(canaryDefinition) + capacity * sizeof(elem_t))) = canaryDefinition;
    
    memset((char*)st->buffer + sizeof(canaryDefinition), poison, capacity * sizeof(elem_t));

#endif

#ifndef CANARY_PROT
    st->buffer = (elem_t*)calloc(capacity, sizeof(elem_t));

    memset(st->buffer, poison, capacity * sizeof(elem_t));

#endif

    st->capacity = capacity;
    st->size = 0; 
    st->name = name;
    st->file = file;
    st->function = function;
    st->line = line;
    st->error = NOERRORS;

#ifdef HASH_PROT
    st->hash = HashRot13(st->buffer, capacity * sizeof(elem_t));
#endif

    output_to_file(st);
    return verification(st);
}

errors stack_push(struct stack* st, const elem_t element) {
    if (st == NULL) {
        my_assert(NULLPTR);
        return NULLPTR;
    }

    errors error = verification(st);
    if (error != NOERRORS)
        return error;

    unsigned int size = st->size;
    unsigned int capacity = st->capacity;
    
    if (size >= capacity) {
        resize(st, capacity, capacity * 2);

        if (st->buffer == NULL) {
            dump(OVERFLOW); 
            return (OVERFLOW);
        }

        ++size;
        st->size = size;

        capacity *= 2;
        st->capacity = capacity;

    #ifdef CANARY_PROT 
        *((elem_t*)((char*)st->buffer + sizeof(canaryDefinition) + (size - 1) * sizeof(elem_t))) = element; 
        
    #endif

    #ifndef CANARY_PROT
        st->buffer[size - 1] = element;
    #endif

    #ifdef HASH_PROT
        st->hash = HashRot13(st->buffer, capacity * sizeof(elem_t));
    #endif

        output_to_file(st);
        return verification(st);   
    }

    ++size;
    st->size = size;

#ifdef CANARY_PROT  
    *((elem_t*)((char*)st->buffer + sizeof(canaryDefinition) + (size - 1) * sizeof(elem_t))) = element; 
        
#endif

#ifndef CANARY_PROT
    st->buffer[size - 1] = element;
#endif

#ifdef HASH_PROT
    st->hash = HashRot13(st->buffer, capacity * sizeof(elem_t));
#endif

    output_to_file(st);
    return verification(st);
}

errors stack_pop(struct stack* st, elem_t* element) {
    if (st == NULL) {
        my_assert(NULLPTR);
        return NULLPTR;
    } 

    errors error = verification(st);
    if (error != NOERRORS)
        return error;

    unsigned int size = st->size;
    unsigned int capacity = st->capacity;

    if (size == 0) {
        dump(UNDERFLOW); 
        return UNDERFLOW;
    }

#ifdef CANARY_PROT
    *element = *((elem_t*)((char*)st->buffer + sizeof(canaryDefinition) + (size - 1) * sizeof(elem_t)));
    memset((char*)st->buffer + sizeof(canaryDefinition) + (size - 1) * sizeof(elem_t), poison, sizeof(elem_t));
#endif

#ifndef CANARY_PROT
    *element = st->buffer[size - 1];
    memset((char*)st->buffer + (size - 1) * sizeof(elem_t), poison, sizeof(elem_t));

#endif
    --size;
    st->size = size;
    
    if (((capacity / 2) >= size) && (capacity > 1)) {
        resize(st, capacity, capacity / 2); 

        if (st->buffer == NULL) {
            dump(BUFFERISNULL);
            return BUFFERISNULL;
        }

        st->capacity = capacity / 2;
    }

#ifdef HASH_PROT
    st->hash = HashRot13(st->buffer, st->capacity * sizeof(elem_t));
#endif

    output_to_file(st);
    return verification(st);
}

errors stack_distructor(struct stack* st) {
    if (st == NULL) {
        dump(WRONGPARAMETERS);
        return WRONGPARAMETERS;
    }

    errors error = verification(st);
    if (error != NOERRORS)
        return error;

    st->size = 0;
    st->capacity = 0;
    free(st->buffer);
    st->buffer = NULL;
    st->hash = 0;

    output_to_file(st);

    return NOERRORS;
}

#ifdef DEBUG

void debug(struct stack* st) {
    if (st == NULL) {
        my_assert(NULLPTR);
        return;
    }

    FILE* stream = fopen(debugFile, "a");

    if (st == NULL) {
        my_assert(NULLPTR);
        return;
    }

    fprintf(stream, "Stack \"%s\" created in file %s in function %s on line %d\n", st->name, st->file, st->function, st->line);
    fprintf(stream, "Size = %d\n", st->size);
    fprintf(stream, "Capacity = %d\n", st->capacity);
    fprintf(stream, "Pointer to buffer = %p\n", st->buffer);
#ifdef HASH_PROT
    fprintf(stream, "Hash = %d\n", st->hash);
#endif
    fprintf(stream, "Error = %d\n", st->error);

#ifdef CANARY_PROT
    fprintf(stream, "Left canary in structure = %llX\n", st->leftCanary);
    fprintf(stream, "Right canary in structure = %llX\n", st->rightCanary);

    fprintf(stream, "Left canary in buffer = %llX\n", *((unsigned long long int*)st->buffer)); 
    fprintf(stream, "Right canary in buffer = %llX\n", *((unsigned long long int*)((char*)st->buffer + sizeof(canaryDefinition) + sizeof(elem_t) * st->capacity)));
    

#if defined INT || defined DOUBLE
    if (st->buffer == NULL) 
        return;

    for (unsigned int i = sizeof(canaryDefinition) / sizeof(elem_t); i < st->capacity + sizeof(canaryDefinition) / sizeof(elem_t); ++i) 
        fprintf(stream, TEXT, i - sizeof(canaryDefinition) / sizeof(elem_t), st->buffer[i]);
    
#endif

#endif

#ifndef CANARY_PROT

#if defined INT || defined DOUBLE
    if (st->buffer == NULL) 
        return;

    for (unsigned int i = 0; i < st->capacity; ++i) 
        fprintf(stream, TEXT, i, st->buffer[i]);
#endif

#endif

    fprintf(stream, "..............................................\n");
    fclose(stream);  
}

#endif

#ifdef DEBUG

#define stack_distor(pointer) if (stack_distructor(pointer) != NOERRORS) return -1; debug(pointer)
#define stack_ctor(pointer, capacity) if (stack_constructor(pointer, capacity, VARNAME(pointer),__FILE__, __PRETTY_FUNCTION__, __LINE__) != NOERRORS) return -1; debug(pointer) 
#define push(pointer, element) if (stack_push(pointer, element) != NOERRORS) return -1; debug(pointer)
#define pop(pointer, element) if (stack_pop(pointer, element) != NOERRORS) return -1; debug(pointer)

#else 

#define stack_distor(pointer) if (stack_distructor(pointer) != NOERRORS) return -1
#define stack_ctor(pointer, capacity) if (stack_constructor(pointer, capacity, VARNAME(pointer),__FILE__, __PRETTY_FUNCTION__, __LINE__) != NOERRORS) return -1
#define push(pointer, element) if (stack_push(pointer, element) != NOERRORS) return -1
#define pop(pointer, element) if (stack_pop(pointer, element) != NOERRORS) return -1

#endif

int main() {
    struct stack st;
    
    stack_ctor(&st, 2);

    elem_t element;

    for(int i = 0; i < 5; ++i) {
        element = {i, "abc"};
        push(&st, element);
    }

    for(int i = 0; i < 5; ++i) {
        pop(&st, &element);
        printf("%d %s\n", element.id, element.str);
    }

    stack_distor(&st);

    return 0;
}