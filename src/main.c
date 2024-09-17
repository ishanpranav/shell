// main.c
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

// References:
//  - https://www.man7.org/linux/man-pages/man3/basename.3.html
//  - https://www.man7.org/linux/man-pages/man3/fgets.3p.html
//  - https://www.man7.org/linux/man-pages/man3/getcwd.3.html
//  - https://www.man7.org/linux/man-pages/man2/signal.2.html

#include <errno.h>
#include <libgen.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include "argument_vector.h"
#include "euler.h"
#include "handler.h"
#include "parser.h"
#include "shell.h"
#include "string_builder.h"

Exception environment_get_current_directory(StringBuilder result)
{
    while (!getcwd(result->buffer, result->capacity))
    {
        switch (errno)
        {
        case ERANGE: break;
        case ENOMEM: return EXCEPTION_OUT_OF_MEMORY;
        default: return 1;
        }

        Exception ex = string_builder_ensure_capacity(
            result,
            result->capacity * 2);

        if (ex)
        {
            return ex;
        }
    }

    result->length = strlen(result->buffer);

    return 0;
}

static void shell_prompt(StringBuilder currentDirectory)
{
    euler_ok(environment_get_current_directory(currentDirectory));

    printf("[nyush %s]$ ", basename(currentDirectory->buffer));
    fflush(stdout);
}

static bool shell_read(StringBuilder result)
{
    char buffer[SHELL_BUFFER_SIZE];

    do
    {
        char* p = fgets(buffer, SHELL_BUFFER_SIZE, stdin);

        euler_ok(string_builder_append_string(result, buffer));

        if (!p)
        {
            return false;
        }
    } 
    while (result->buffer[result->length - 1] != '\n');

    return true;
}

int main()
{
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);

    struct Parser state;
    struct StringBuilder line;
    struct StringBuilder currentDirectory;

    euler_ok(parser(&state));
    euler_ok(string_builder(&line, 0));
    euler_ok(string_builder(&currentDirectory, 0));

    Instruction instruction;

    do
    {
        shell_prompt(&currentDirectory);

        if (!shell_read(&line))
        {
            break;
        }

        parser_parse(&state, &line);

        if (state.faulted)
        {
            fprintf(stderr, "Error: invalid command\n");

            continue;
        }

        instruction = state.first;
    } 
    while (!instruction || instruction->execute(&state.jobs, instruction));

    finalize_string_builder(&line);
    finalize_string_builder(&currentDirectory);
    finalize_parser(&state);

    return 0;
}
