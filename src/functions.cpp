#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "../includes/functions.h"

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

    output_to_file(st);

    return NOERRORS;
}

#ifdef DEBUG

void debug(struct stack* st) {
    FILE* stream = fopen(debugFile, "a");

    if (st == NULL) {
        my_assert(NULLPTR);
        return;
    }

    fprintf(stream, "Stack \"%s\" created in file %s in function %s on line %d\n", st->name, st->file, st->function, st->line);
    fprintf(stream, "Size = %d\n", st->size);
    fprintf(stream, "Capacity = %d\n", st->capacity);
    fprintf(stream, "Pointer to buffer = %p\n", st->buffer);
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



