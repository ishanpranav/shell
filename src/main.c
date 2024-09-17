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

int main()
{
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);

    struct Parser state;

    euler_ok(parser(&state));

    Instruction instruction;
    size_t lineCapacity = 4;
    String line = malloc(lineCapacity);
    
    euler_assert(line);

    size_t currentDirectoryCapacity = 4;
    String currentDirectory = malloc(currentDirectoryCapacity);

    euler_assert(currentDirectory);

    do
    {
        errno = 0;
        
        while (!getcwd(currentDirectory, currentDirectoryCapacity))
        {
            euler_assert(errno == ERANGE);

            currentDirectoryCapacity *= 2;
            currentDirectory = realloc(
                currentDirectory, 
                currentDirectoryCapacity);

            euler_assert(currentDirectory);
        }

        printf("[nyush %s]$ ", basename(currentDirectory));
        fflush(stdout);

        ssize_t length = getline(&line, &lineCapacity, stdin);

        if (length == -1)
        {
            break;
        }

        parser_parse(&state, line, length);
        
        if (state.faulted)
        {
            instruction = NULL;
            
            fprintf(stderr, "Error: invalid command\n");

            continue;
        }

        instruction = state.first;
    } 
    while (!instruction || instruction->execute(&state.jobs, instruction));

    free(line);
    free(currentDirectory);
    finalize_parser(&state);
}
