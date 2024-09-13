// nyush.c
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

// References:
//  - https://www.man7.org/linux/man-pages/man3/fgets.3p.html
//  - https://www.man7.org/linux/man-pages/man3/getcwd.3.html

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "euler.h"
#include "string_builder.h"

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
    char buffer[4];

    do
    {
        char* p = fgets(buffer, 4, stdin);
        
        euler_ok(string_builder_append_string(result, buffer));

        if (!p)
        {
            return false;
        }
    }
    while (result->buffer[result->length - 1] != '\n');

    string_builder_trim_right(result);

    return true;
}

int main()
{
    struct StringBuilder line;
    struct StringBuilder currentDirectory;

    euler_ok(string_builder(&line, 4));
    euler_ok(string_builder(&currentDirectory, 4));

    do
    {
        // printf(">> %s\n", line.buffer);
        shell_prompt(&currentDirectory);
    }
    while (shell_read(&line));

    finalize_string_builder(&line);
    finalize_string_builder(&currentDirectory);

    return 0;
}
