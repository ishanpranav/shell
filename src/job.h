// job.h
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

#include <sys/types.h>
#include "instruction.h"

struct Job
{
    pid_t pid;
    struct Instruction* first;
};

typedef struct Job* Job;
