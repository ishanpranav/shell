// execute_handler.h
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../handler.h"

HandleResult execute_handler(ArgumentVector args)
{
    pid_t pid = fork();

    euler_assert(pid >= 0);

    if (pid)
    {
        waitpid(-1, NULL, 0);

        return HANDLE_RESULT_CONTINUE;
    }

    if (strchr(args->buffer[0], '/'))
    {
        execv(args->buffer[0], args->buffer);
    }
    else
    {
        execv(args->buffer[0], args->buffer);

        struct StringBuilder path; // `exec*` is called: no need to finalize

        euler_ok(string_builder(&path, 0));
        euler_ok(string_builder_append_string(&path, "/usr/bin/"));
        euler_ok(string_builder_append_string(&path, args->buffer[0]));
        execv(path.buffer, args->buffer);
    }
    
    fprintf(stderr, "Error: invalid program\n");

    return HANDLE_RESULT_EXIT;
}
