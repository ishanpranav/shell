// change_directory_handler.h
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../handler.h"

HandleResult change_directory_handler(ArgumentVector args)
{
    if (strcmp(args->buffer[0], "cd") != 0)
    {
        return HANDLE_RESULT_NONE;
    }

    if (args->count != 2)
    {
        fprintf(stderr, "Error: invalid command\n");

        return HANDLE_RESULT_CONTINUE;
    }

    if (chdir(args->buffer[1]) == -1)
    {
        fprintf(stderr, "Error: invalid directory\n");
    }

    return HANDLE_RESULT_CONTINUE;
}
