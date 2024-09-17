// argument_vector.c
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

// References:
//  - https://www.man7.org/linux/man-pages/man3/strdup.3.html

#include <string.h>
#include "argument_vector.h"
#define ARGUMENT_VECTOR_DELIMITERS " \r\n"

Exception argument_vector(ArgumentVector instance, size_t capacity)
{
    if (capacity < 4)
    {
        capacity = 4;
    }

    instance->buffer = malloc((capacity + 1) * sizeof instance->buffer);

    if (!instance->buffer)
    {
        return EXCEPTION_OUT_OF_MEMORY;
    }

    instance->count = 0;
    instance->capacity = capacity;
    instance->buffer[0] = NULL;

    return 0;
}

Exception argument_vector_ensure_capacity(
    ArgumentVector instance,
    size_t capacity)
{
    if (instance->capacity >= capacity)
    {
        return 0;
    }

    size_t newCapacity = (instance->capacity - 1) * 2;

    if (capacity > newCapacity)
    {
        newCapacity = capacity;
    }

    String* newBuffer;
    size_t newSize = (newCapacity + 1) * sizeof * newBuffer;

    newBuffer = realloc(instance->buffer, newSize);

    if (!newBuffer)
    {
        return EXCEPTION_OUT_OF_MEMORY;
    }

    instance->capacity = newCapacity;
    instance->buffer = newBuffer;

    return 0;
}

Exception argument_vector_tokenize(ArgumentVector instance, String value)
{
    size_t count = instance->count;

    for (String token = strtok(value, ARGUMENT_VECTOR_DELIMITERS);
        token;
        token = strtok(NULL, ARGUMENT_VECTOR_DELIMITERS))
    {
        Exception ex = argument_vector_ensure_capacity(instance, count + 1);

        if (ex)
        {
            return ex;
        }

        String clone = strdup(token);

        if (!clone)
        {
            return EXCEPTION_OUT_OF_MEMORY;
        }

        instance->buffer[count] = clone;
        count++;
    }

    instance->buffer[count] = NULL;
    instance->count = count;
    
    return 0;
}

void argument_vector_clear(ArgumentVector instance)
{
    while (instance->count)
    {
        instance->count--;

        free(instance->buffer[instance->count]);
    }

    instance->count = 0;
    instance->buffer[0] = NULL;
}

void finalize_argument_vector(ArgumentVector instance)
{
    argument_vector_clear(instance);
    free(instance->buffer);
}
