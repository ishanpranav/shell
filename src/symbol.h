// symbol.h
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

// References:
//  - https://en.wikipedia.org/wiki/Recursive_descent_parser

enum Symbol
{
    SYMBOL_NONE,
    SYMBOL_CHANGE_DIRECTORY,
    SYMBOL_EXIT,
    SYMBOL_FOREGROUND,
    SYMBOL_JOBS,
    SYMBOL_READ,
    SYMBOL_WRITE,
    SYMBOL_APPEND,
    SYMBOL_PIPE,
    SYMBOL_STRING
};

typedef enum Symbol Symbol;
