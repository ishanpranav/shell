// exit_handler.c
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

#include "handler.h"

bool exit_handler(
    JobCollection jobs, 
    EULER_UNUSED Instruction instruction)
{
    if (jobs->count)
    {
        fprintf(stderr, "Error: there are suspended jobs\n");

        return true;
    }

    return false;
}
