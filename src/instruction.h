// instruction.h
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

#include "instruction_type.h"

struct Instruction
{
    enum InstructionType type;
    char* read;
    char* write;
    char* append;
    char** arguments;
    struct Instruction* nextPipe;
};

typedef struct Instruction* Instruction;
