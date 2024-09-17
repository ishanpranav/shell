// string_builder.h
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

// https://github.com/ishanpranav/codebook/blob/master/lib/string_builder.h

#ifndef STRING_BUILDER_4d4532d6f2db4b4088af8db287de600b
#define STRING_BUILDER_4d4532d6f2db4b4088af8db287de600b
#include <stddef.h>
#include "euler.h"
#include "exception.h"

/** Represents a mutable string of characters. */
struct StringBuilder
{
    char* buffer;
    size_t length;
    size_t capacity;
};

/** Represents a mutable string of characters. */
typedef struct StringBuilder* StringBuilder;

/**
 * Initializes an `StringBuilder` instance.
 * 
 * @param instance the `StringBuilder` instance.
 * @param capacity the required minimum capacity.
 * @return `EXCEPTION_OUT_OF_MEMORY` if there is not enough memory to complete
 *         the operation; otherwise `0`.
*/
Exception string_builder(StringBuilder instance, size_t capacity);

/**
 * Ensures that the capacity of this instance of `StringBuilder` is at least the
 * specified value.
 * 
 * @param instance the `StringBuilder` instance.
 * @param value    the required minimum capacity.
 * @return `EXCEPTION_OUT_OF_MEMORY` if there is not enough memory to complete
 *         the operation; otherwise `0`.
*/
Exception string_builder_ensure_capacity(
    StringBuilder instance,
    size_t capacity);

/**
 * Appends a copy of the specified string to this instance.
 * 
 * @param instance the `StringBuilder` instance.
 * @param value    the string to append.
 * @return An exception; otherwise `0`.
*/
Exception string_builder_append_string(StringBuilder instance, String value);

/**
 * Converts the value of an `StringBuilder` to a new `String`.
 * 
 * @param instance the `StringBuilder` instance.
 * @return A new string whose value is the same as this instance. The caller is
 *         responsible.
*/
String string_builder_to_string(StringBuilder instance);

/**
 * Frees all resources.
 * 
 * @param instance the `StringBuilder` instance. This method corrupts the
 *                 `instance` argument.
*/
void finalize_string_builder(StringBuilder instance);

#endif
