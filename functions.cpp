#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <assert.h>
#include "functions.h"

void stack_constructor(struct stack* stackName, unsigned int size, unsigned int capacity) {
    if ((size > capacity) || (stackName == NULL))
        stack_dump(stackName, WRONGPARAMETERS, __FILE__, __PRETTY_FUNCTION__, __LINE__);

    if (capacity == 0)
        capacity = 1;

    stackName->buffer = (double*)calloc(capacity, sizeof(double));
    stackName->capacity = capacity;
    stackName->size = size;

    for (unsigned int i = 0; i < capacity; ++i) 
        stackName->buffer[i] = NAN;  

    output_to_file(stackName);
}

void resize(struct stack* stackName, unsigned int oldCapacity, unsigned int newCapacity) {
    if (stackName == NULL)
        stack_dump(stackName, NULLPTR, __FILE__, __PRETTY_FUNCTION__, __LINE__);

    stackName->buffer = (double*)realloc(stackName->buffer, newCapacity * sizeof(double));

    if (stackName->buffer != NULL) {
        for (unsigned int i = oldCapacity; i < newCapacity; ++i) 
            stackName->buffer[i] = NAN;
    }
}

void push(struct stack* stackName, double element) {
    if (stackName == NULL)
        stack_dump(stackName, NULLPTR, __FILE__, __PRETTY_FUNCTION__, __LINE__);

    unsigned int size = stackName->size;
    unsigned int capacity = stackName->capacity;
    
    if (size >= capacity) {
        resize(stackName, capacity, capacity * 2);

        if (stackName->buffer == NULL) 
            stack_dump(stackName, OVERFLOW, __FILE__, __PRETTY_FUNCTION__, __LINE__);

        ++size;
        stackName->size = size;
        capacity *= 2;
        stackName->capacity = capacity;

        stackName->buffer[size - 1] = element;

        output_to_file(stackName);
        return;
    }

    ++size;
    stackName->size = size;

    stackName->buffer[size - 1] = element;

    output_to_file(stackName);
}

void pop(struct stack* stackName, double* element) {
    if (stackName == NULL || element == NULL)
        stack_dump(stackName, NULLPTR, __FILE__, __PRETTY_FUNCTION__, __LINE__);

    unsigned int size = stackName->size;
    unsigned int capacity = stackName->capacity;

    if (size == 0) 
        stack_dump(stackName, UNDERFLOW, __FILE__, __PRETTY_FUNCTION__, __LINE__);

    *element = stackName->buffer[size - 1];

    stackName->buffer[size - 1] = NAN;

    --size;
    stackName->size = size;
    
    if ((capacity / 2 == size) && (capacity > 1)) {
        resize(stackName, capacity, capacity / 2);

        if (stackName->buffer == NULL)
            stack_dump(stackName, BUFFERISNULL, __FILE__, __PRETTY_FUNCTION__, __LINE__);

        stackName->capacity = capacity / 2;
    }

    output_to_file(stackName);
}

void stack_distructor(struct stack* stackName) {
    if (stackName == NULL)
        stack_dump(stackName, WRONGPARAMETERS, __FILE__, __PRETTY_FUNCTION__, __LINE__);

    stackName->size = 0;
    stackName->capacity = 0;
    free(stackName->buffer);
    stackName->buffer = NULL;

    output_to_file(stackName);
}

void verification(struct stack* stackName) {
    if (stackName == NULL)
        stack_dump(stackName, NULLPTR, __FILE__, __PRETTY_FUNCTION__, __LINE__);

    FILE* file = fopen("data.txt", "r");
    if (file == NULL)
       stack_dump(stackName, FILEWASNTOPEN, __FILE__, __PRETTY_FUNCTION__, __LINE__);

    unsigned int capacity = stackName->capacity;
    unsigned int size = stackName->size;
    double* buffer = stackName->buffer;

    unsigned int dataCapacity = 0;
    unsigned int dataSize = 0;
    double* dataBuffer = NULL;

    fscanf(file, "%d\n%d\n%p\n", &dataSize, &dataCapacity, &dataBuffer);

    if (dataSize != size) 
        stack_dump(stackName, MISMATCHSIZE, __FILE__, __PRETTY_FUNCTION__, __LINE__);
     
    if (dataCapacity != capacity)
        stack_dump(stackName, MISMATCHCAPACITY, __FILE__, __PRETTY_FUNCTION__, __LINE__);

    if (dataBuffer != buffer) {
        stack_dump(stackName, MISMATCHPOINTERTOBUFFER, __FILE__, __PRETTY_FUNCTION__, __LINE__);
    }

    if ((dataBuffer != NULL) && (buffer != NULL)) {
        for (unsigned int i = 0; i < capacity; ++i) {
            double dataElement = NAN;
            fscanf(file, "%lf\n", &dataElement);

            if (!compare_two_numbers(dataElement, stackName->buffer[i]))
                stack_dump(stackName, MISMATCHELEMENT, __FILE__, __PRETTY_FUNCTION__, __LINE__);
        }
    }

    stack_dump(stackName, NOERRORS, __FILE__, __PRETTY_FUNCTION__, __LINE__);

    fclose(file);
}

void output_to_file(struct stack* stackName) {
    if (stackName == NULL)
        stack_dump(stackName, NULLPTR, __FILE__, __PRETTY_FUNCTION__, __LINE__);

    FILE* file = fopen("data.txt", "w");
    if (file == NULL)
       stack_dump(stackName, FILEWASNTOPEN, __FILE__, __PRETTY_FUNCTION__, __LINE__); 

    unsigned int capacity = stackName->capacity;
    unsigned int size = stackName->size;
    double* buffer = stackName->buffer;

    fprintf(file, "%d\n%d\n%p\n", size, capacity, buffer);

    if (buffer != NULL) {
        for (unsigned int i = 0; i < capacity; ++i) {
            fprintf(file, "%lf\n", buffer[i]);
        }
    }

    fclose(file);
}

void stack_dump(struct stack* stackName, errors error, const char* file, const char* function, const int line) {
    if (stackName == NULL || file == NULL || function == NULL) {
       printf("In file %s in function %s in line %d error %d was happened.\nLook also data.txt\n", __FILE__, __PRETTY_FUNCTION__, __LINE__, STACKDUMP);
       assert(0);
    }

    if (error == NOERRORS) {    
        FILE* stream = fopen("log.txt", "w");
        if (stream == NULL) {
            printf("log.txt wasn`t open look info below:\n");
            printf("No errors in stack code %d\n", NOERRORS);
            printf("Size = %d\n", stackName->size);
            printf("Capacity = %d\n", stackName->capacity);
            printf("Pointer to buffer = %p\n", stackName->buffer);

            if (stackName->buffer != NULL) {
                for (unsigned int i = 0; i < stackName->capacity; ++i) 
                    printf("data[%d] = %lf\n", i, stackName->buffer[i]);
            }

            return;
        }

        fprintf(stream, "No errors in stack code %d\n", NOERRORS);
        fprintf(stream, "Size = %d\n", stackName->size);
        fprintf(stream, "Capacity = %d\n", stackName->capacity);
        fprintf(stream, "Pointer to buffer = %p\n", stackName->buffer);

        if (stackName->buffer != NULL) {
            for (unsigned int i = 0; i < stackName->capacity; ++i) 
                fprintf(stream, "data[%d] = %lf\n", i, stackName->buffer[i]);
        }

        fclose(stream);
        return;
    }

    FILE* stream = fopen("log.txt", "w");
    if (stream == NULL) {
        printf("log.txt wasn`t open look info below:\n");
        printf("In file %s in function %s in line %d error %d was happened.\n Look also data.txt\n", file, function, line, error);
        printf("Size = %d\n", stackName->size);
        printf("Capacity = %d\n", stackName->capacity);
        printf("Pointer to buffer = %p\n", stackName->buffer);

        if (stackName->buffer != NULL) {
            for (unsigned int i = 0; i < stackName->capacity; ++i) 
                printf("data[%d] = %lf\n", i, stackName->buffer[i]);
        }

        assert(0);
    }

    fprintf(stream, "In file %s in function %s in line %d error %d was happened.\nLook also data.txt\n", file, function, line, error);
    fprintf(stream, "Size = %d\n", stackName->size);
    fprintf(stream, "Capacity = %d\n", stackName->capacity);
    fprintf(stream, "Pointer to buffer = %p\n", stackName->buffer);

    if (stackName->buffer != NULL) {
        for (unsigned int i = 0; i < stackName->capacity; ++i) 
            fprintf(stream, "data[%d] = %lf\n", i, stackName->buffer[i]);
    }

    fclose(stream);
    printf("Look at the log.txt\n");
    assert(0);
}

bool compare_two_numbers(double a, double b) {
    if (isnan(a) && isnan(b)) 
        return true;

    double epsilon = 0.00001;

    if (fabs(a - b) <= epsilon) 
        return true;
    
    return false;
}

