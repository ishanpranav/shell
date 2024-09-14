// parser.c
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

#include <stdbool.h>
#include <string.h>
#include "euler.h"
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

void parser(Parser instance, ArgumentVector args)
{
    instance->args = args;
    instance->current = SYMBOL_NONE;
    instance->index = 0;
    instance->faulted = false;
    instance->instruction.type = INSTRUCTION_TYPE_NONE;
    instance->instruction.read = NULL;
    instance->instruction.write = NULL;
    instance->instruction.append = NULL;
    instance->instruction.arguments = NULL;
    instance->instruction.nextPipe = NULL;
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

static bool parser_expect(Parser instance, Symbol symbol)
{
    if (parser_accept(instance, symbol))
    {
        return true;
    }

    instance->faulted = true;

    return false;
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
    parser_parse_command_name(instance);

    while (instance->current == SYMBOL_STRING) 
    {
        parser_parse_argument(instance);
    }
}

static void parser_parse_file_name(Parser instance)
{
    parser_expect(instance, SYMBOL_STRING);
}

static void parser_parse_terminate(Parser instance)
{
    if (parser_accept(instance, SYMBOL_WRITE) || 
        parser_accept(instance, SYMBOL_APPEND)) 
    {
        parser_expect(instance, SYMBOL_STRING);

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

void parser_parse_command(Parser instance)
{
    parser_next(instance);

    if (parser_accept(instance, SYMBOL_NONE)) 
    {
        return;
    }

    if (parser_accept(instance, SYMBOL_CHANGE_DIRECTORY)) 
    {
        parser_parse_argument(instance);
        parser_expect(instance, SYMBOL_NONE);

        return;
    }

    if (parser_accept(instance, SYMBOL_EXIT) || 
        parser_accept(instance, SYMBOL_JOBS)) 
    {
        parser_expect(instance, SYMBOL_NONE);

        instance->instruction.type = INSTRUCTION_TYPE_EXIT;

        return;
    }

    if (parser_accept(instance, SYMBOL_FOREGROUND)) 
    {
        parser_parse_argument(instance);
        parser_expect(instance, SYMBOL_NONE);

        return;
    }

    parser_parse_command_text(instance);

    if (parser_accept(instance, SYMBOL_READ)) 
    {
        parser_parse_file_name(instance);

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
        parser_parse_file_name(instance);
    }

    parser_expect(instance, SYMBOL_NONE);
}

void finalize_parser(Parser instance)
{
    while (instance->instruction.nextPipe)
    {
        Instruction next = instance->instruction.nextPipe->nextPipe;

        free(instance->instruction.nextPipe);

        instance->instruction.nextPipe = next;
    }
}
