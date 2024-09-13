// euler.h
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

// https://github.com/ishanpranav/codebook/blob/master/lib/euler.h

#ifndef EULER_2363b2bd9868485c9651b89fbe7082ba
#define EULER_2363b2bd9868485c9651b89fbe7082ba
#define EULER_MACRO do {
#define EULER_END_MACRO } while (0)
#define euler_ok(ex) EULER_MACRO \
    Exception _ex = (ex); \
    if (_ex) { \
    fprintf(stderr, "Error: %d at %s line %d.\n", _ex, __FILE__, __LINE__); \
    exit(_ex); } EULER_END_MACRO
#define euler_assert(condition) EULER_MACRO if (!(condition)) { \
    fprintf(stderr, "Faulted: %s line %d.\n", __FILE__, __LINE__); \
    exit(1); } EULER_END_MACRO

/** Represents text as a zero-terminated sequence of characters. */
typedef char* String;

/**
 * Creates a new instance of `String` with the same value as a specified
 * `String`.
 *
 * @param instance the `String` instance to copy.
 * @return A new string with the same value, or `NULL` if the process is out of
 *         memory. The caller is responsible.
 */
String string_clone(String instance);

#endif
