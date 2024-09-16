// parser.c
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

#include <stdbool.h>
#include <string.h>
#include "euler.h"
#include "handler.h"
#include "parser.h"

char* INVALID_CHARS = "><*!`'\"|";
String SYMBOL_STRINGS[SYMBOLS] =
{
    [SYMBOL_NONE] = NULL,
    [SYMBOL_CHANGE_DIRECTORY] = "cd",
    [SYMBOL_EXIT] = "exit",
    [SYMBOL_FOREGROUND] = "fg",
    [SYMBOL_JOBS] = "jobs",
    [SYMBOL_READ] = "<",
    [SYMBOL_WRITE] = ">",
    [SYMBOL_APPEND] = ">>",
    [SYMBOL_PIPE] = "|",
    [SYMBOL_STRING] = NULL,
    [SYMBOL_INVALID] = NULL
};

static Instruction parser_add(Parser instance, Handler handler)
{
    Instruction result = calloc(1, sizeof * result);

    result->execute = handler;

    if (instance->last)
    {
        instance->last->nextPipe = result;
        instance->last = result;
    }
    else
    {
        instance->first = result;
        instance->last = result;
    }

    return result;
}

static void parser_reset(Parser instance)
{
    instance->current = SYMBOL_NONE;
    instance->index = 0;
    instance->faulted = false;

    while (instance->first)
    {
        Instruction next = instance->first->nextPipe;

        free(instance->first);

        instance->first = next;
    }

    instance->last = NULL;
}

void parser(Parser instance, ArgumentVector args)
{
    instance->args = args;
    instance->first = NULL;

    parser_reset(instance);
}

static Symbol parser_classify_token(String value)
{
    for (Symbol symbol = 0; symbol < SYMBOLS; symbol++)
    {
        if (!SYMBOL_STRINGS[symbol])
        {
            continue;
        }

        if (strcmp(value, SYMBOL_STRINGS[symbol]) == 0)
        {
            return symbol;
        }
    }

    for (char* p = INVALID_CHARS; *p; p++)
    {
        if (strchr(value, *p))
        {
            return SYMBOL_INVALID;
        }
    }

    return SYMBOL_STRING;
}

static void parser_next(Parser instance)
{
    ArgumentVector args = instance->args;

    if (instance->index >= args->count)
    {
        instance->current = SYMBOL_NONE;

        return;
    }

    instance->current = parser_classify_token(args->buffer[instance->index]);
    instance->index++;
}

static bool parser_accept(Parser instance, Symbol symbol)
{
    if (instance->current == symbol)
    {
        parser_next(instance);

        return true;
    }

    return false;
}

static void parser_expect(Parser instance, Symbol symbol)
{
    if (parser_accept(instance, symbol))
    {
        return;
    }

    instance->faulted = true;
}

static void parser_parse_argument(Parser instance)
{
    parser_expect(instance, SYMBOL_STRING);
}

static void parser_parse_command_name(Parser instance)
{
    parser_expect(instance, SYMBOL_STRING);
}

static void parser_parse_command_text(Parser instance)
{
    size_t offset = instance->index - 1;
    size_t length = 1;

    parser_parse_command_name(instance);

    while (instance->current == SYMBOL_STRING)
    {
        parser_parse_argument(instance);

        length++;
    }

    Instruction added = parser_add(instance, execute_handler);

    added->length = length;
    added->payload.arguments = instance->args->buffer + offset;
}

static void parser_parse_file_name(Parser instance)
{
    parser_expect(instance, SYMBOL_STRING);
}

static void parser_parse_terminate(Parser instance)
{
    if (parser_accept(instance, SYMBOL_WRITE))
    {
        size_t offset = instance->index - 1;

        parser_parse_file_name(instance);

        instance->last->write = instance->args->buffer[offset];

        return;
    }

    if (parser_accept(instance, SYMBOL_APPEND))
    {
        size_t offset = instance->index - 1;
        
        parser_parse_file_name(instance);

        instance->last->append = instance->args->buffer[offset];

        return;
    }

    parser_expect(instance, SYMBOL_NONE);
}

static void parser_parse_recursive(Parser instance)
{
    parser_expect(instance, SYMBOL_PIPE);
    parser_parse_command_text(instance);

    if (instance->current == SYMBOL_PIPE)
    {
        parser_parse_recursive(instance);
    }
    else
    {
        parser_parse_terminate(instance);
    }

    parser_expect(instance, SYMBOL_NONE);
}

static void parser_parse_command(Parser instance)
{
    if (parser_accept(instance, SYMBOL_NONE))
    {
        return;
    }

    if (parser_accept(instance, SYMBOL_CHANGE_DIRECTORY))
    {
        parser_parse_argument(instance);
        parser_expect(instance, SYMBOL_NONE);

        Instruction added = parser_add(instance, change_directory_handler);

        added->payload.argument = instance->args->buffer[1];

        return;
    }

    if (parser_accept(instance, SYMBOL_EXIT))
    {
        parser_expect(instance, SYMBOL_NONE);
        parser_add(instance, exit_handler);

        return;
    }

    if (parser_accept(instance, SYMBOL_JOBS))
    {
        parser_expect(instance, SYMBOL_NONE);
        parser_add(instance, jobs_handler);
        
        return;
    }

    if (parser_accept(instance, SYMBOL_FOREGROUND))
    {
        parser_parse_argument(instance);
        parser_expect(instance, SYMBOL_NONE);
        
        Instruction added = parser_add(instance, foreground_handler);

        added->payload.argument = instance->args->buffer[1];

        return;
    }

    parser_parse_command_text(instance);

    if (parser_accept(instance, SYMBOL_READ))
    {
        size_t offset = instance->index - 1;

        parser_parse_file_name(instance);

        instance->last->read = instance->args->buffer[offset];

        if (instance->current == SYMBOL_PIPE)
        {
            parser_parse_recursive(instance);
        }
        else
        {
            parser_parse_terminate(instance);
        }

        parser_expect(instance, SYMBOL_NONE);

        return;
    }

    if (instance->current == SYMBOL_PIPE)
    {
        parser_parse_recursive(instance);
        parser_expect(instance, SYMBOL_NONE);

        return;
    }

    parser_parse_terminate(instance);

    if (parser_accept(instance, SYMBOL_READ))
    {
        size_t offset = instance->index - 1;

        parser_parse_file_name(instance);

        instance->last->read = instance->args->buffer[offset];
    }

    parser_expect(instance, SYMBOL_NONE);
}

void parser_parse(Parser instance)
{
    parser_reset(instance);
    parser_next(instance);
    parser_parse_command(instance);
}

void finalize_parser(Parser instance)
{
    parser_reset(instance);
}
