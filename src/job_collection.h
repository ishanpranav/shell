// job_collection.h
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

#ifndef JOB_COLLECTION_1cb8a579912440e2b04aa4d31f016ed4
#define JOB_COLLECTION_1cb8a579912440e2b04aa4d31f016ed4
#include <sys/types.h>
#include <stdbool.h>
#include <stddef.h>
#include "euler.h"
#include "exception.h"

union InstructionPayload
{
    char* argument;
    char** arguments;
};

struct JobCollection;

struct Instruction
{
    int descriptors[2];
    size_t length;
    char* text;
    char* read;
    char* write;
    char* append;
    union InstructionPayload payload;
    struct Instruction* nextPipe;

    bool (*execute)(struct JobCollection* jobs, struct Instruction* instance);
};

struct Job
{
    pid_t pid;
    struct Instruction* first;
};

struct JobCollection
{
    size_t count;
    size_t capacity;
    struct Job* items;
    struct Instruction* freeList;
    struct Instruction** aliasReference;
};

typedef struct Instruction* Instruction;
typedef struct Job* Job;
typedef struct JobCollection* JobCollection;

Exception job(Job instance, pid_t pid, Instruction first);
void finalize_job(Job instance);

Exception job_collection(JobCollection instance, size_t capacity);

Exception job_collection_ensure_capacity(
    JobCollection instance,
    size_t capacity);

Exception job_collection_add(
    JobCollection instance,
    pid_t value,
    Instruction first);

Exception job_collection_remove_at(JobCollection instance, size_t index);
void job_collection_free_instruction(JobCollection instance, Instruction value);
void job_collection_garbage_collect(JobCollection instance);
void finalize_job_collection(JobCollection instance);

#endif
