#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <assert.h>
#include "functions.h"

#define my_assert(condition, error) processor_of_errors(condition, error, __FILE__, __PRETTY_FUNCTION__, __LINE__)
#define dump(condition, error) stack_dump(condition, st, error, __FILE__, __PRETTY_FUNCTION__, __LINE__)

#ifdef INT
    #define FORMAT "%ld\n"
    #define TEXT "data[%d] = %ld\n"
    #define POISON 0XDEADDEAD

#else

    #define POISON NAN
    #define FORMAT "%lf\n"
    #define TEXT "data[%d] = %lf\n"
#endif

const char* logFile = "log.txt";
const char* dataFile = "data.txt";

static void processor_of_errors(bool condition, errors error, const char* function, const char* name, const int line) { 
    if (condition != 0) 
        return;
    
    switch (error) {

    case FILEWASNTOPEN:
        printf("In file %s function %s on line %d: file wasn`t open, programm finished\n", function, name, line);
        break;

    case NULLPTR:
        printf("In file %s function %s on line %d: NULL was given as a parameter, programm finished\n", function, name, line);
        break;

    default:
        break;
    }

    assert(0);   
}

static bool file_is_open(FILE* stream) {
    if (stream == NULL) 
        return false;

    return true;   
}

static void info_in_logfile(struct stack* st, FILE* stream) {
    my_assert(st != NULL && stream != NULL, NULLPTR);

    fprintf(stream, "Stack \"%s\" created in file %s in function %s on line %d\n", st->name, st->file, st->function, st->line);
    fprintf(stream, "Size = %d\n", st->size);
    fprintf(stream, "Capacity = %d\n", st->capacity);
    fprintf(stream, "Pointer to buffer = %p\n", st->buffer);

    if (st->buffer == NULL) 
        return;

    for (unsigned int i = 0; i < st->capacity; ++i) {
    #ifdef INT
        if (st->buffer[i] == POISON) {
            fprintf(stream, "data[%d] = %ld = %lX\n", i, st->buffer[i], st->buffer[i]);
            continue;
        }

    #endif

        fprintf(stream, TEXT, i, st->buffer[i]);
    }
    
}

static void stack_dump(bool condition, struct stack* st, errors error, const char* file, const char* function, const int line) {
    if (condition == 0) 
        return;

    my_assert(st != NULL && file != NULL && function != NULL, NULLPTR);

    FILE* stream = fopen(logFile, "w");
    my_assert(file_is_open(stream), FILEWASNTOPEN);

    fprintf(stream, "In file %s in function %s on line %d error %d was happened.\nLook also at data.txt\n", file, function, line, error);
    info_in_logfile(st, stream);

    fclose(stream);
    printf("Look at the log.txt\n");
    assert(0);   
}

static void output_to_file(struct stack* st) {
    my_assert(st != NULL, NULLPTR);

    FILE* file = fopen(dataFile, "w");
    my_assert(file_is_open(file), FILEWASNTOPEN); 

    unsigned int capacity = st->capacity;
    unsigned int size = st->size;
    elem_t* buffer = st->buffer;

    fprintf(file, "%d\n%d\n%p\n", size, capacity, buffer);

    if (buffer != NULL) {
        for (unsigned int i = 0; i < capacity; ++i) 
            fprintf(file, FORMAT, buffer[i]);
        
    }

    fclose(file);
}

static void fill_poison(struct stack* st, const unsigned int leftBorder, const unsigned int rightBorder) {
    my_assert(st != NULL, NULLPTR);

    for (unsigned int i = leftBorder; i < rightBorder; ++i) 
        st->buffer[i] = POISON;
}

static bool compare_two_numbers(elem_t a, elem_t b) {
#ifdef INT
    return a == b;

#else

    if (isnan(a) && isnan(b)) 
        return true;

    elem_t epsilon = 0.00001;

    if (fabs(a - b) <= epsilon) 
        return true;
    
    return false;
#endif
}

static void verification(struct stack* st) {
    my_assert(st != NULL, NULLPTR);

    FILE* file = fopen(dataFile, "r");
    my_assert(file_is_open(file), FILEWASNTOPEN);

    unsigned int capacity = st->capacity;
    unsigned int size = st->size;
    elem_t* buffer = st->buffer;

    unsigned int dataCapacity = 0;
    unsigned int dataSize = 0;
    elem_t* dataBuffer = NULL;

    fscanf(file, "%d\n%d\n%p\n", &dataSize, &dataCapacity, &dataBuffer);

    dump(dataSize != size, MISMATCHSIZE); 
     
    dump(dataCapacity != capacity, MISMATCHCAPACITY);

    dump(dataBuffer != buffer, MISMATCHPOINTERTOBUFFER); 

    if ((dataBuffer != NULL) && (buffer != NULL)) {
        for (unsigned int i = 0; i < capacity; ++i) {
            elem_t dataElement = POISON;

            fscanf(file, FORMAT, &dataElement);

            dump(!compare_two_numbers(dataElement, st->buffer[i]), MISMATCHELEMENT);
        }
    }

    fclose(file);
}

static void resize(struct stack* st, const unsigned int oldCapacity, const unsigned int newCapacity) {
    my_assert(st != NULL, NULLPTR);

    st->buffer = (elem_t*)realloc(st->buffer, newCapacity * sizeof(elem_t));

    if (st->buffer != NULL) 
        fill_poison(st, oldCapacity, newCapacity);
    
}

void stack_constructor(struct stack* st, const int cp, const char* name, const char* file, const char* function, const int line) {
    dump((st == NULL) || (cp < 0) || (file == NULL) || (function == NULL), WRONGPARAMETERS);

    unsigned int capacity = (unsigned int)cp;

    if (capacity == 0)
        capacity = 1;

    st->buffer = (elem_t*)calloc(capacity, sizeof(elem_t));
    st->capacity = capacity;
    st->size = 0; 
    st->name = name;
    st->file = file;
    st->function = function;
    st->line = line;

    fill_poison(st, 0, capacity);

    output_to_file(st);
    verification(st);
}

void push(struct stack* st, const elem_t element) {
    my_assert(st != NULL, NULLPTR);

    verification(st);

    unsigned int size = st->size;
    unsigned int capacity = st->capacity;
    
    if (size >= capacity) {
        resize(st, capacity, capacity * 2);

        dump(st->buffer == NULL, OVERFLOW); 

        ++size;
        st->size = size;

        capacity *= 2;
        st->capacity = capacity;

        st->buffer[size - 1] = element;

        output_to_file(st);
        verification(st);
        return;
    }

    ++size;
    st->size = size;

    st->buffer[size - 1] = element;

    output_to_file(st);
    verification(st);
}

elem_t pop(struct stack* st) {
    my_assert(st != NULL, NULLPTR);

    verification(st);

    unsigned int size = st->size;
    unsigned int capacity = st->capacity;

    dump(size == 0, UNDERFLOW); 

    elem_t element = st->buffer[size - 1];

    st->buffer[size - 1] = POISON;

    --size;
    st->size = size;
    
    if (((capacity / 2) >= size) && (capacity > 1)) {
        resize(st, capacity, capacity / 2); 

        dump(st->buffer == NULL, BUFFERISNULL);

        st->capacity = capacity / 2;
    }

    output_to_file(st);
    verification(st);
    return element;
}

void stack_distructor(struct stack* st) {
    dump(st == NULL, WRONGPARAMETERS);

    verification(st);

    st->size = 0;
    st->capacity = 0;
    free(st->buffer);
    st->buffer = NULL;

    output_to_file(st);
    verification(st);
}



