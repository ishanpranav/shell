// nyush.c
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

// References:
//  - https://www.man7.org/linux/man-pages/man3/getcwd.3.html

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "euler.h"
#include "string_builder.h"

Exception shell_get_current_directory(StringBuilder result)
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

int main()
{
    struct StringBuilder currentDirectory;

    euler_ok(string_builder(&currentDirectory, 4));
    euler_ok(shell_get_current_directory(&currentDirectory));

    String substring = NULL;

    for (size_t i = currentDirectory.length - 1; i > 0; i--)
    {
        if (currentDirectory.buffer[i - 1] == '/')
        {
            substring = currentDirectory.buffer + i;

            break;
        }
    }

    printf("[nyush %s]$ ", substring);
    fflush(stdout);
    finalize_string_builder(&currentDirectory);

    return 0;
}
