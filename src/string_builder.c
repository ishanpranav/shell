// string_builder.c
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

// https://github.com/ishanpranav/codebook/blob/master/lib/string_builder.c

#include <ctype.h>
#include <stdint.h>
#include <string.h>
#include "string_builder.h"

Exception string_builder(StringBuilder instance, size_t capacity)
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

    instance->length = 0;
    instance->capacity = capacity;
    instance->buffer[0] = '\0';

    return 0;
}

Exception string_builder_ensure_capacity(
    StringBuilder instance,
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

    char* newBuffer;
    size_t newSize = (newCapacity + 1) * sizeof * newBuffer;

    newBuffer = realloc(instance->buffer, newSize);

    if (!newBuffer)
    {
        return EXCEPTION_OUT_OF_MEMORY;
    }

    newBuffer[instance->length] = '\0';
    instance->capacity = newCapacity;
    instance->buffer = newBuffer;

    return 0;
}

Exception string_builder_append_string(StringBuilder instance, String value)
{
    size_t length = strlen(value);
    size_t newLength = instance->length + length;
    Exception ex = string_builder_ensure_capacity(instance, newLength);

    if (ex)
    {
        return ex;
    }

    memcpy(instance->buffer + instance->length, value, length);

    instance->length += length;
    instance->buffer[instance->length] = '\0';

    return 0;
}

String string_builder_to_string(StringBuilder instance)
{
    String result = malloc((instance->length + 1) * sizeof * result);

    if (!result)
    {
        return NULL;
    }

    memcpy(result, instance->buffer, instance->length);

    result[instance->length] = '\0';

    return result;
}

void finalize_string_builder(StringBuilder instance)
{
    free(instance->buffer);

    instance->buffer = NULL;
    instance->length = 0;
    instance->capacity = 0;
}
