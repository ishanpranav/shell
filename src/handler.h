// handler.h
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

#include "argument_vector.h"
#include "handle_result.h"

typedef HandleResult (*Handler)(ArgumentVector args);

HandleResult exit_handler(ArgumentVector args);
HandleResult change_directory_handler(ArgumentVector args);
HandleResult execute_handler(ArgumentVector args);
