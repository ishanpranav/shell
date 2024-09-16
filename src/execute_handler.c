// execute_handler.c
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

// References:
//  - https://www.man7.org/linux/man-pages/man3/exec.3.html
//  - https://www.man7.org/linux/man-pages/man2/fork.2.html
//  - https://www.man7.org/linux/man-pages/man2/open.2.html
//  - https://www.man7.org/linux/man-pages/man2/pipe.2.html
//  - https://www.man7.org/linux/man-pages/man3/stdin.3.html
//  - https://www.man7.org/linux/man-pages/man2/wait.2.html
//  - https://www.gnu.org/software/libc/manual/html_node/Permission-Bits.html

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "handler.h"
#include "shell.h"

static void execute_handler_finalize_arguments(String values[], size_t count)
{
    for (size_t i = 0; i < count; i++)
    {
        free(values[i]);
    }

    free(values);
}

static bool execute_handler_interpret(Instruction instruction, bool hasPipe)
{
    String* arguments = malloc((instruction->length + 1) * sizeof * arguments);

    euler_assert(arguments);

    for (size_t i = 0; i < instruction->length; i++)
    {
        arguments[i] = strdup(instruction->payload.arguments[i]);

        euler_assert(arguments[i]);
    }

    arguments[instruction->length] = NULL;

    int pipeDescriptors[2];

    if (instruction->nextPipe)
    {
        euler_assert(pipe(pipeDescriptors) != -1);
        euler_assert(pipeDescriptors[0] != -1);
        euler_assert(pipeDescriptors[1] != -1);
    }

    pid_t pid = fork();

    euler_assert(pid >= 0);

    if (pid)
    {
        if (hasPipe && instruction->nextPipe)
        {
            euler_assert(close(pipeDescriptors[1]) != -1);

            instruction->nextPipe->descriptor = pipeDescriptors[0];
        }

        int status;

        waitpid(-1, &status, WUNTRACED);

        if (WIFSTOPPED(status))
        {
            fprintf(stderr, "STOPPED\n");
        }
        
        execute_handler_finalize_arguments(arguments, instruction->length);

        return true;
    }

    if (instruction->write)
    {
        int descriptor = creat(instruction->write, S_IRUSR | S_IWUSR);

        euler_assert(descriptor != -1);
        euler_assert(dup2(descriptor, STDOUT_FILENO) != -1);
        euler_assert(close(descriptor) != -1);
    }
    else if (instruction->append)
    {
        int descriptor = open(
            instruction->append,
            O_APPEND | O_CREAT | O_WRONLY,
            S_IRUSR | S_IWUSR);

        euler_assert(descriptor != -1);
        euler_assert(dup2(descriptor, STDOUT_FILENO) != -1);
        euler_assert(close(descriptor) != -1);
    }

    if (instruction->read)
    {
        int descriptor = open(instruction->read, O_RDONLY, S_IRUSR);

        if (descriptor == -1)
        {
            execute_handler_finalize_arguments(arguments, instruction->length);
            fprintf(stderr, "Error: invalid file\n");

            return false;
        }

        euler_assert(dup2(descriptor, STDIN_FILENO) != -1);
        euler_assert(close(descriptor) != -1);
    }

    if (hasPipe)
    {
        euler_assert(dup2(instruction->descriptor, STDIN_FILENO) != -1);

        if (instruction->nextPipe)
        {
            euler_assert(dup2(pipeDescriptors[1], STDOUT_FILENO) != -1);
            euler_assert(close(pipeDescriptors[0]) != -1);
        }
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

    fprintf(stderr, "Error: invalid program\n");
    execute_handler_finalize_arguments(arguments, instruction->length);

    return false;
}

bool execute_handler(Instruction instruction)
{
    bool hasPipe = instruction->nextPipe;

    for (; instruction; instruction = instruction->nextPipe)
    {
        if (!execute_handler_interpret(instruction, hasPipe))
        {
            return false;
        }
    }

    return true;
}
