// job.c
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

#include "instruction.h"

Exception job(Job instance, pid_t pid, Instruction first)
{
    Exception ex = string_builder(&instance->text, 0);

    if (ex)
    {
        return ex;
    }
    
    for (Instruction current = first; current; current = current->nextPipe)
    {
        String* arguments = current->payload.arguments;

        for (String* argument = arguments; *argument; argument++)
        {
            string_builder_append_string(&instance->text, *argument);
            string_builder_append_string(&instance->text, " ");
        }
    }

    instance->pid = pid;
    instance->first = first;

    return 0;
}

void finalize_job(Job instance)
{
    finalize_string_builder(&instance->text);
}
