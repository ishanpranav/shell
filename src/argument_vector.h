// argument_vector.h
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

#ifndef ARGUMENT_VECTOR_57fd44ec18234de6ad3bdfba493e374b
#define ARGUMENT_VECTOR_57fd44ec18234de6ad3bdfba493e374b
#include <stddef.h>
#include "euler.h"
#include "exception.h"

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

Exception argument_vector_tokenize(ArgumentVector instance, String value);
void argument_vector_clear(ArgumentVector instance);
void finalize_argument_vector(ArgumentVector instance);

#endif
