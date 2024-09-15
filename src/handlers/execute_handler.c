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
#include "../handler.h"
#include "../shell.h"

static void execute_handler_finalize_arguments(String values[], size_t count)
{
    for (size_t i = 0; i < count; i++)
    {
        free(values[i]);
    }

    free(values);
}

static void execute_handler_pipe(String arguments[])
{
    // int pipeDescriptors[2];

    // euler_assert(pipe(pipeDescriptors) != -1);

    // pid_t pid = fork();

    // euler_assert(pid >= 0);

    // if (pid)
    // {
    //     euler_assert(close(pipeDescriptors[0]) != -1);

    //     int duplicate = dup(pipeDescriptors[1]);

    //     euler_assert(duplicate != -1);
    //     euler_assert(dup2(pipeDescriptors[1], STDOUT_FILENO) != -1);
    //     euler_assert(close(duplicate) != -1);

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
    // euler_assert(close(pipeDescriptors[1]) != -1);
// }
// else
// {
//     euler_assert(close(pipeDescriptors[1]) != -1);

//     char buffer[SHELL_BUFFER_SIZE];

//     while (read(pipeDescriptors[0], buffer, SHELL_BUFFER_SIZE) > 0)
//     {
//         write(STDOUT_FILENO, buffer, SHELL_BUFFER_SIZE);
//     }

//     write(STDOUT_FILENO, "\n", 1);

//     euler_assert(close(pipeDescriptors[0]) != -1);
// }
}

static void execute_handler_redirect(int* state, int source, int target)
{
    *state = dup(target);

    euler_assert(*state != -1);
    euler_assert(dup2(source, target) != -1);
}

static void execute_handler_unredirect(int* state, int source, int target)
{
    if (source == -1)
    {
        return;
    }

    euler_assert(dup2(*state, target) != -1);
    euler_assert(close(*state) != -1);
    euler_assert(close(source) != -1);
}

static bool execute_handler_interpret(Instruction instruction)
{
    int inDescriptor = -1;
    int outDescriptor = -1;
    int inState;
    int outState;
    String* arguments = malloc((instruction->length + 1) * sizeof * arguments);

    euler_assert(arguments);

    for (size_t i = 0; i < instruction->length; i++)
    {
        arguments[i] = string_clone(instruction->payload.arguments[i]);

        euler_assert(arguments[i]);
    }

    arguments[instruction->length] = NULL;

    if (instruction->write)
    {
        outDescriptor = creat(instruction->write, S_IRUSR | S_IWUSR);

        euler_assert(outDescriptor != -1);
        execute_handler_redirect(&outState, outDescriptor, STDOUT_FILENO);
    }
    else if (instruction->append)
    {
        outDescriptor = open(
            instruction->append,
            O_APPEND | O_CREAT | O_WRONLY,
            S_IRUSR | S_IWUSR);

        euler_assert(outDescriptor != -1);
        execute_handler_redirect(&outState, outDescriptor, STDOUT_FILENO);
    }

    if (instruction->read)
    {
        inDescriptor = open(instruction->read, O_RDONLY, S_IRUSR);

        if (inDescriptor == -1)
        {
            execute_handler_finalize_arguments(arguments, instruction->length);
            fprintf(stderr, "Error: invalid file\n");

            return true;
        }

        execute_handler_redirect(&inState, inDescriptor, STDIN_FILENO);
    }

    pid_t pid = fork();

    euler_assert(pid >= 0);

    if (pid)
    {
        waitpid(-1, NULL, 0);
        execute_handler_unredirect(&inState, inDescriptor, STDIN_FILENO);
        execute_handler_unredirect(&outState, outDescriptor, STDOUT_FILENO);
        execute_handler_finalize_arguments(arguments, instruction->length);

        return true;
    }

    // for (Instruction pipe = instruction; pipe; pipe = pipe->nextPipe)
    // {
    //     printf("-> '%s'\n", pipe->payload.arguments[0]);
    // }

    execute_handler_pipe(arguments);
    execute_handler_finalize_arguments(arguments, instruction->length);

    return false;
}

bool execute_handler(Instruction instruction)
{
    // for (; instruction; instruction = instruction->nextPipe)
    // {
    return    execute_handler_interpret(instruction);
    // }

    // return true;
}
