// parser.h
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

// References:
//  - https://en.wikipedia.org/wiki/Recursive_descent_parser

#include <stdbool.h>
#include "argument_vector.h"
#include "job_collection.h"
#include "symbol.h"

struct Parser
{
    bool faulted;
    enum Symbol current;
    size_t index;
    struct JobCollection jobs;
    struct ArgumentVector arguments;
    char* text;
    struct Instruction* first;
    struct Instruction* last;
};

typedef struct Parser* Parser;

Exception parser(Parser instance);
Exception parser_parse(Parser instance, String value, size_t length);
void finalize_parser(Parser instance);
