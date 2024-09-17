// jobs_handler.c
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

#include "handler.h"

bool jobs_handler(JobCollection jobs, EULER_UNUSED Instruction instruction)
{
    for (size_t i = 0; i < jobs->count; i++)
    {
        printf("[%zu] %s\n", i + 1, jobs->items[i].first->text);
    }

    return true;
}
