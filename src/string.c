// string.c
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

// https://github.com/ishanpranav/codebook/blob/master/lib/string.c

#include <string.h>
#include "euler.h"

String string_clone(String instance)
{
    size_t size = strlen(instance) + 1;
    String result = malloc(size * sizeof * result);

    if (!result)
    {
        return NULL;
    }

    return memcpy(result, instance, size);
}
