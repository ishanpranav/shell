// exit_handler.h
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

#include <stdio.h>
#include <string.h>
#include "../handler.h"

HandleResult exit_handler(ArgumentVector args)
{
    if (strcmp(args->buffer[0], "exit") != 0)
    {
        return HANDLE_RESULT_NONE;
    }

    if (args->count != 1)
    {
        fprintf(stderr, "Error: invalid command\n");

        return HANDLE_RESULT_CONTINUE;
    }

    return HANDLE_RESULT_EXIT;
}
