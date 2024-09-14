// handler.h
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

#include <stdbool.h>
#include "argument_vector.h"
#include "instruction.h"

typedef bool (*Handler)(Instruction instruction);

bool exit_handler(Instruction instruction);
bool change_directory_handler(Instruction instruction);
bool foreground_handler(Instruction instruction);
bool jobs_handler(Instruction instruction);
bool execute_handler(Instruction instruction);
