// job_collection.h
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

#include <stddef.h>
#include "exception.h"
#include "job.h"

struct JobCollection
{
    size_t count;
    size_t capacity;
    struct Job* items;
};

typedef struct JobCollection* JobCollection;

Exception job_collection(JobCollection instance, size_t capacity);
JobCollection job_collection_get_default();

Exception job_collection_ensure_capacity(
    JobCollection instance, 
    size_t capacity);

Exception job_collection_add(JobCollection instance, int value);
Exception job_collection_remove_at(JobCollection instance, size_t index);
void finalize_job_collection(JobCollection instance);
