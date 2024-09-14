// instruction_type.h
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

enum InstructionType
{
    INSTRUCTION_TYPE_NONE = 0,
    INSTRUCTION_TYPE_CHANGE_DIRECTORY,
    INSTRUCTION_TYPE_EXIT,
    INSTRUCTION_TYPE_FOREGROUND,
    INSTRUCTION_TYPE_JOBS,
    INSTRUCTION_TYPE_EXECUTE 
};

typedef enum InstructionType InstructionType;
