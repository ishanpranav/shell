// parser.h
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

// References:
//  - https://en.wikipedia.org/wiki/Recursive_descent_parser

#include <stdbool.h>
#include "argument_vector.h"
#include "instruction.h"
#include "symbol.h"

struct Parser
{
    bool faulted;
    enum Symbol current;
    size_t index;
    struct JobCollection jobs;
    struct ArgumentVector* args;
    struct Instruction* first;
    struct Instruction* last;
};

typedef struct Parser* Parser;

Exception parser(Parser instance, ArgumentVector args);
void parser_parse(Parser instance);
void finalize_parser(Parser instance);
