// change_directory_handler.c
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

// References:
//  - https://www.man7.org/linux/man-pages/man2/chdir.2.html

#include <stdio.h>
#include <unistd.h>
#include "../handler.h"

bool change_directory_handler(Instruction instruction)
{
    if (chdir(instruction->payload.argument) == -1)
    {
        fprintf(stderr, "Error: invalid directory\n");
    }

    return true;
}
