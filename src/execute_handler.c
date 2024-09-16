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

static void execute_handler_finalize_descriptors(Instruction first)
{
    for (Instruction p = first; p; p = p->nextPipe)
    {
        euler_assert(close(p->descriptors[0]) != -1);
        euler_assert(close(p->descriptors[1]) != -1);
    }
}

static void execute_handler_finalize_arguments(String values[])
{
    for (String* p = values; *p; p++)
    {
        free(*p);
    }

    free(values);
}

static bool execute_handler_run(Instruction current)
{
    String* arguments = malloc((current->length + 1) * sizeof * arguments);

    euler_assert(arguments);

    for (size_t i = 0; i < current->length; i++)
    {
        arguments[i] = strdup(current->payload.arguments[i]);

        euler_assert(arguments[i]);
    }

    arguments[current->length] = NULL;

    pid_t pid = fork();

    euler_assert(pid >= 0);

    if (pid)
    {
        int status;

        waitpid(pid, &status, WUNTRACED);

        if (WIFSTOPPED(status))
        {

        }

        return true;
    }

    if (current->write)
    {
        int descriptor = creat(current->write, S_IRUSR | S_IWUSR);

        euler_assert(descriptor != -1);
        euler_assert(dup2(descriptor, STDOUT_FILENO) != -1);
        euler_assert(close(descriptor) != -1);
    }
    else if (current->append)
    {
        int descriptor = open(
            current->append,
            O_APPEND | O_CREAT | O_WRONLY,
            S_IRUSR | S_IWUSR);

        euler_assert(descriptor != -1);
        euler_assert(dup2(descriptor, STDOUT_FILENO) != -1);
        euler_assert(close(descriptor) != -1);
    }

    if (current->read)
    {
        int descriptor = open(current->read, O_RDONLY, S_IRUSR);

        if (descriptor == -1)
        {
            // execute_handler_finalize_arguments(arguments, current->length);
            fprintf(stderr, "Error: invalid file\n");

            return false;
        }

        euler_assert(dup2(descriptor, STDIN_FILENO) != -1);
        euler_assert(close(descriptor) != -1);
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
    execute_handler_finalize_arguments(arguments);

    return false;
}

static bool execute_handler_interpret(Instruction first, Instruction current)
{
    // int pipeDescriptors[2];

    // if (instruction->nextPipe)
    // {
    //     euler_assert(pipe(pipeDescriptors) != -1);
    //     euler_assert(pipeDescriptors[0] != -1);
    //     euler_assert(pipeDescriptors[1] != -1);
    // }

    pid_t pid = fork();

    euler_assert(pid >= 0);

    if (pid)
    {
        // if (hasPipe && instruction->nextPipe)
        // {
        //     euler_assert(close(pipeDescriptors[1]) != -1);

        //     instruction->nextPipe->descriptor = pipeDescriptors[0];
        // }

        int status;

        waitpid(-1, &status, WUNTRACED);

        if (WIFSTOPPED(status))
        {
            fprintf(stderr, "STOPPED\n");
        }
        
        // execute_handler_finalize_arguments(arguments, current->length);
        execute_handler_finalize_descriptors(first);

        return true;
    }
    // if (hasPipe)
    // {
    //     euler_assert(dup2(instruction->descriptor, STDIN_FILENO) != -1);

    //     if (instruction->nextPipe)
    //     {
    //         euler_assert(dup2(pipeDescriptors[1], STDOUT_FILENO) != -1);
    //         euler_assert(close(pipeDescriptors[0]) != -1);
    //     }
    // }
    
    if (current->descriptors[0] != -1) 
    {
        euler_assert(dup2(current->descriptors[0], STDIN_FILENO) != -1);
    }

    if (current->nextPipe)
    {
        int descriptor = current->nextPipe->descriptors[1];

        euler_assert(dup2(descriptor, STDOUT_FILENO) != -1);
    }

    // execute_handler_finalize_descriptors(first);

    // execute_handler_finalize_arguments(arguments, current->length);

    return false;
}

bool execute_handler(Instruction instruction)
{
    if (!instruction->nextPipe) 
    {
        return execute_handler_run(instruction);
    }

    for (Instruction p = instruction->nextPipe; p; p = p->nextPipe)
    {
        euler_assert(pipe(p->descriptors) != -1);
    }

    for (Instruction p = instruction; p; p = p->nextPipe)
    {
        if (!execute_handler_interpret(instruction, p))
        {
            return false;
        }
    }

    return true;
}
