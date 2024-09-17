// main.c
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

// References:
//  - https://www.man7.org/linux/man-pages/man3/basename.3.html
//  - https://www.man7.org/linux/man-pages/man3/fgets.3p.html
//  - https://www.man7.org/linux/man-pages/man3/getcwd.3.html
//  - https://www.man7.org/linux/man-pages/man3/getline.3.html
//  - https://www.man7.org/linux/man-pages/man2/signal.2.html

#include <errno.h>
#include <libgen.h>
#include <signal.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include "argument_vector.h"
#include "euler.h"
#include "handler.h"
#include "parser.h"

String environment_get_current_directory()
{
    size_t capacity = 4;
    String buffer = malloc(capacity);

    if (!buffer)
    {
        return NULL;
    }

    errno = 0;
    
    while (!getcwd(buffer, capacity))
    {
        if (errno != ERANGE)
        {
            return NULL;
        }

        capacity *= 2;
        buffer = realloc(buffer, capacity);

        if (!buffer)
        {
            return NULL;
        }
    }

    return buffer;
}

int main()
{
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);

    struct Parser state;

    euler_ok(parser(&state));

    Instruction instruction;

    do
    {
        String currentDirectory = environment_get_current_directory();

        euler_assert(currentDirectory);
        printf("[nyush %s]$ ", basename(currentDirectory));
        free(currentDirectory);
        fflush(stdout);

        String line = NULL;
        size_t length = 0;
        ssize_t read = getline(&line, &length, stdin);

        if (read == -1)
        {
            free(line);

            break;
        }

        parser_parse(&state, line, read);
        free(line);

        if (state.faulted)
        {
            fprintf(stderr, "Error: invalid command\n");

            continue;
        }

        instruction = state.first;
    } 
    while (!instruction || instruction->execute(&state.jobs, instruction));

    finalize_parser(&state);
}
