// execute_handler.c
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../handler.h"

bool execute_handler(Instruction instruction)
{
    pid_t pid = fork();

    euler_assert(pid >= 0);

    if (pid)
    {
        waitpid(-1, NULL, 0);

        return true;
    }

    String* arguments = malloc(instruction->length * sizeof * arguments);

    euler_assert(arguments);

    for (size_t i = 0; i < instruction->length; i++)
    {
        arguments[i] = string_clone(instruction->payload.arguments[i]);

        euler_assert(arguments[i]);
    }

    if (strchr(arguments[0], '/'))
    {
        execv(arguments[0], arguments);
    }
    else
    {
        execv(arguments[0], arguments);

        struct StringBuilder path;

        euler_ok(string_builder(&path, 0));
        euler_ok(string_builder_append_string(&path, "/usr/bin/"));
        euler_ok(string_builder_append_string(&path, arguments[0]));
        execv(path.buffer, arguments);
        finalize_string_builder(&path);
    }
    
    for (size_t i = 0; i < instruction->length; i++)
    {
        free(arguments[i]);
    }
    
    free(arguments);
    fprintf(stderr, "Error: invalid program\n");

    return false;
}
