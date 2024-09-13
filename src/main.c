// main.c
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

// References:
//  - https://www.man7.org/linux/man-pages/man3/fgets.3p.html
//  - https://www.man7.org/linux/man-pages/man2/fork.2.html
//  - https://www.man7.org/linux/man-pages/man3/getcwd.3.html
//  - https://www.man7.org/linux/man-pages/man3/exec.3.html
//  - https://www.man7.org/linux/man-pages/man1/wait.1p.html

#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "euler.h"
#include "string_builder.h"
#define SHELL_DELIMITERS " \r\n"
#define SHELL_BUFFER_SIZE 4

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
    } while (result->buffer[result->length - 1] != '\n');

    // string_builder_trim_right(result);

    return true;
}

static String* shell_tokenize(StringBuilder value)
{
    size_t i = 0;
    String* result = malloc(sizeof(String) * 4);

    euler_assert(result);

    for (String token = strtok(value->buffer, SHELL_DELIMITERS);
        token;
        token = strtok(NULL, SHELL_DELIMITERS))
    {
        size_t size = strlen(token) + 1;

        result[i] = malloc(size);

        euler_assert(result[i]);
        memcpy(result[i], token, size);

        i++;
    }

    value->length = 0;
    result[i] = NULL;

    return result;
}

int main()
{
    struct StringBuilder line;
    struct StringBuilder currentDirectory;

    euler_ok(string_builder(&line, SHELL_BUFFER_SIZE));
    euler_ok(string_builder(&currentDirectory, SHELL_BUFFER_SIZE));

    for (;;)
    {
        line.length = 0;
        shell_prompt(&currentDirectory);

        if (!shell_read(&line))
        {
            break;
        }

        pid_t pid = fork();

        euler_assert(pid >= 0);

        if (pid)
        {
            wait(&pid);
        }
        else
        {
            String* args = shell_tokenize(&line);

            execvp(args[0], args);

            euler_assert(false);
        }
    }

    finalize_string_builder(&line);
    finalize_string_builder(&currentDirectory);

    return 0;
}
