// job_collection.c
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

// https://github.com/ishanpranav/codebook/blob/master/lib/list.c

#include <sys/wait.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "euler.h"
#include "job_collection.h"

Exception job_collection(JobCollection instance, size_t capacity)
{
    if (capacity < 4)
    {
        capacity = 4;
    }

    instance->items = malloc(capacity * sizeof * instance->items);

    if (!instance->items)
    {
        return EXCEPTION_OUT_OF_MEMORY;
    }

    instance->count = 0;
    instance->capacity = capacity;
    instance->freeList = NULL;
    instance->aliasReference = NULL;

    return 0;
}

Exception job_collection_ensure_capacity(
    JobCollection instance,
    size_t capacity)
{
    if (instance->capacity >= capacity)
    {
        return 0;
    }

    size_t newCapacity = instance->capacity * 2;

    if (capacity > newCapacity)
    {
        newCapacity = capacity;
    }

    struct Job* newItems = realloc(
        instance->items, 
        newCapacity * sizeof * newItems);

    if (!newItems)
    {
        return EXCEPTION_OUT_OF_MEMORY;
    }

    instance->capacity = newCapacity;
    instance->items = newItems;

    return 0;
}

Exception job_collection_add(
    JobCollection instance, 
    pid_t pid,
    Instruction first)
{
    Exception ex = job_collection_ensure_capacity(
        instance, 
        instance->count + 1);

    if (ex)
    {
        return ex;
    }

    instance->items[instance->count].pid = pid;
    instance->items[instance->count].first = first;
    
    if (instance->aliasReference)
    {
        if (*instance->aliasReference == first)
        {
            *instance->aliasReference = NULL;
        }
    }

    instance->count++;

    return 0;
}

Exception job_collection_remove_at(JobCollection instance, size_t index)
{
    if (index >= instance->count)
    {
        return EXCEPTION_ARGUMENT_OUT_OF_RANGE;
    }

    instance->count--;

    if (index == instance->count)
    {
        return 0;
    }

    memmove(
        instance->items + index, 
        instance->items + index + 1, 
        (instance->count - index) * sizeof * instance->items);

    return 0;
}

void job_collection_free_instruction(JobCollection instance, Instruction value)
{
    if (!instance->freeList)
    {
        instance->freeList = value;

        return;
    }

    instance->freeList->nextPipe = value;
}

void job_collection_garbage_collect(JobCollection instance)
{
    while (instance->freeList)
    {
        Instruction next = instance->freeList->nextPipe;

        if (instance->freeList->text)
        {
            free(instance->freeList->text);
        
            instance->freeList->text = NULL;
        }

        free(instance->freeList);

        instance->freeList = next;
    }
}

void finalize_job_collection(JobCollection instance)
{
    instance->count = 0;

    free(instance->items);
    job_collection_garbage_collect(instance);

    instance->items = NULL;
    instance->capacity = 0;
}
