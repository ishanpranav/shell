// foreground_handler.c
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

// References:
//  - https://www.man7.org/linux/man-pages/man2/kill.2.html
//  - https://www.man7.org/linux/man-pages/man3/strtol.3.html
//  - https://www.gnu.org/software/libc/manual/html_node/Continuing-Stopped-Jobs.html
//  - https://www.gnu.org/software/libc/manual/html_node/Foreground-and-Background.html
//  - https://web.stanford.edu/class/cs110/summer-2021/lecture-notes/lecture-08

#include <sys/wait.h>
#include <signal.h>
#include "handler.h"

bool foreground_handler(JobCollection jobs, Instruction instruction)
{
    unsigned long long job = strtoull(instruction->payload.argument, NULL, 10);

    if (job < 1 || job > jobs->count)
    {
        fprintf(stderr, "Error: invalid job\n");

        return true;
    }
    
    struct Job item = jobs->items[job - 1];
    
    euler_ok(job_collection_remove_at(jobs, job - 1));
    kill(item.pid, SIGCONT);
    
    int status;
    
    euler_assert(waitpid(item.pid, &status, WUNTRACED) != -1);
    
    if (WIFSTOPPED(status))
    {
        euler_ok(job_collection_add(jobs, item.pid, item.first));
    }
    else
    {
        job_collection_free_instruction(jobs, item.first);
    }

    return true;
}
