// handler.h
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

#include <stdbool.h>
#include "argument_vector.h"
#include "job_collection.h"

typedef bool (*Handler)(JobCollection jobs, Instruction instruction);

bool exit_handler(JobCollection jobs, Instruction instruction);
bool change_directory_handler(JobCollection jobs, Instruction instruction);
bool foreground_handler(JobCollection jobs, Instruction instruction);
bool jobs_handler(JobCollection jobs, Instruction instruction);
bool execute_handler(JobCollection jobs, Instruction instruction);
