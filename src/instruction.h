// instruction.h
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

#ifndef INSTRUCTION_1cb8a579912440e2b04aa4d31f016ed4
#define INSTRUCTION_1cb8a579912440e2b04aa4d31f016ed4

union InstructionPayload
{
    char* argument;
    char** arguments;
};

struct Instruction
{
    int descriptor;
    size_t length;
    char* read;
    char* write;
    char* append;
    union InstructionPayload payload;
    struct Instruction* nextPipe;

    bool (*execute)(struct Instruction* instance);
};

typedef struct Instruction* Instruction;

#endif
