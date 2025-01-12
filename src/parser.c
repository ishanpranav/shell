// parser.c
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

#include <ctype.h>
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

    result->descriptors[0] = -1;
    result->descriptors[1] = -1;
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
        result->text = instance->text;
    }

    return result;
}

static void parser_reset(Parser instance)
{
    instance->current = SYMBOL_NONE;
    instance->index = 0;
    instance->faulted = false;

    if (instance->first)
    {
        job_collection_free_instruction(&instance->jobs, instance->first);

        instance->first = NULL;
    }

    instance->last = NULL;
}

Exception parser(Parser instance)
{
    Exception ex = job_collection(&instance->jobs, 0);

    if (ex)
    {
        return ex;
    }

    ex = argument_vector(&instance->arguments, 0);

    if (ex)
    {
        return ex;
    }

    instance->first = NULL;
    instance->jobs.aliasReference = &instance->first;
    instance->text = NULL;

    parser_reset(instance);

    return 0;
}

static Symbol parser_classify(String value)
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
    if (instance->index >= instance->arguments.count)
    {
        instance->current = SYMBOL_NONE;

        return;
    }

    String token = instance->arguments.buffer[instance->index];

    instance->current = parser_classify(token);
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
    
    if (instance->first)
    {
        job_collection_free_instruction(&instance->jobs, instance->first);

        instance->first = NULL;
    }
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
    added->payload.arguments = instance->arguments.buffer + offset;
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

        instance->last->write = instance->arguments.buffer[offset];

        return;
    }

    if (parser_accept(instance, SYMBOL_APPEND))
    {
        size_t offset = instance->index - 1;
        
        parser_parse_file_name(instance);

        instance->last->append = instance->arguments.buffer[offset];

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

        added->payload.argument = instance->arguments.buffer[1];

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

        added->payload.argument = instance->arguments.buffer[1];

        return;
    }

    parser_parse_command_text(instance);

    if (parser_accept(instance, SYMBOL_READ))
    {
        size_t offset = instance->index - 1;

        parser_parse_file_name(instance);

        instance->last->read = instance->arguments.buffer[offset];

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

        instance->last->read = instance->arguments.buffer[offset];
    }

    parser_expect(instance, SYMBOL_NONE);
}

Exception parser_parse(Parser instance, String value, size_t length)
{
    parser_reset(instance);
    argument_vector_clear(&instance->arguments);
    
    while (isspace(value[length - 1]))
    {
        length--;
    }

    if (!length)
    {
        return 0;
    }

    String duplicate = malloc(length + 1);

    if (!duplicate)
    {
        return EXCEPTION_OUT_OF_MEMORY;
    }

    memcpy(duplicate, value, length);

    duplicate[length] = '\0';
    
    Exception ex = argument_vector_tokenize(&instance->arguments, value);

    if (ex)
    {
        return ex;
    }

    instance->text = duplicate;

    if (!instance->arguments.count)
    {
        return 0;
    }

    job_collection_garbage_collect(&instance->jobs);
    parser_next(instance);
    parser_parse_command(instance);

    return 0;
}

void finalize_parser(Parser instance)
{
    parser_reset(instance);
    finalize_job_collection(&instance->jobs);
    finalize_argument_vector(&instance->arguments);
}
