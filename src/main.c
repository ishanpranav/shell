// main.c
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

// References:
//  - https://www.man7.org/linux/man-pages/man2/chdir.2.html
//  - https://www.man7.org/linux/man-pages/man3/exec.3.html
//  - https://www.man7.org/linux/man-pages/man3/fgets.3p.html
//  - https://www.man7.org/linux/man-pages/man2/fork.2.html
//  - https://www.man7.org/linux/man-pages/man3/getcwd.3.html
//  - https://www.man7.org/linux/man-pages/man2/signal.2.html
//  - https://www.man7.org/linux/man-pages/man1/wait.1p.html

#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "argument_vector.h"
#include "euler.h"
#include "handler.h"
#include "parser.h"
#include "string_builder.h"
#define SHELL_BUFFER_SIZE 4

static Handler SHELL_HANDLERS[] = 
{
    exit_handler,
    change_directory_handler,
    execute_handler
};

Exception environment_current_directory(StringBuilder result)
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
    euler_ok(environment_current_directory(currentDirectory));

    String substring = NULL;

    for (size_t i = currentDirectory->length - 1; i != SIZE_MAX; i--)
    {
        if (currentDirectory->buffer[i - 1] == '/')
        {
            substring = currentDirectory->buffer + i;

            break;
        }
    }

    printf("[nyush %s]$ ", substring);
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

    struct StringBuilder line;
    struct StringBuilder currentDirectory;
    struct ArgumentVector args;

    euler_ok(string_builder(&line, 0));
    euler_ok(string_builder(&currentDirectory, 0));
    euler_ok(argument_vector(&args, 0));

    for (;;)
    {
        shell_prompt(&currentDirectory);

        if (!shell_read(&line))
        {
            break;
        }

        argument_vector_clear(&args);
        euler_ok(argument_vector_tokenize(&args, &line));

        if (args.count == 0)
        {
            continue;
        }

        parser(&args);
        command();

        if (iserror()) {
            fprintf(stderr, "Error: invalid command\n");
        }

        // HandleResult handleResult;

        // for (Handler* handler = SHELL_HANDLERS; *handler; handler++)
        // {
        //     handleResult = (*handler)(&args);

        //     if (handleResult)
        //     {
        //         break;
        //     }
        // }

        // if (handleResult == HANDLE_RESULT_CONTINUE)
        // {
        //     continue;
        // }

        // if (handleResult == HANDLE_RESULT_EXIT)
        // {
        //     break;
        // }
    }

    finalize_string_builder(&line);
    finalize_string_builder(&currentDirectory);
    finalize_argument_vector(&args);

    return 0;
}
