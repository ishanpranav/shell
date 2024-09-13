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

/** Represents text as a zero-terminated sequence of characters. */
typedef char* String;

#endif
