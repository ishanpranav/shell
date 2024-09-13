// argument_vector.h
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

#include <stddef.h>
#include "euler.h"
#include "exception.h"
#include "string_builder.h"

struct ArgumentVector
{
    char** buffer;
    size_t count;
    size_t capacity;
};

typedef struct ArgumentVector* ArgumentVector;

Exception argument_vector(ArgumentVector instance, size_t capacity);

Exception argument_vector_ensure_capacity(
    ArgumentVector instance,
    size_t capacity);

Exception argument_vector_tokenize(
    ArgumentVector instance, 
    StringBuilder value);

void argument_vector_clear(ArgumentVector instance);
void finalize_argument_vector(ArgumentVector instance);
