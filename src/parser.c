// parser.c
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

#include <stdbool.h>
#include <string.h>
#include "euler.h"
#include "parser.h"
#include "symbol.h"

Symbol sym;
ArgumentVector args;
size_t i;
char* invalidChars = "><|*!`'\"";
bool error;

bool iserror() {
    return error;
}

void parser(ArgumentVector a) {
    sym = SYMBOL_NONE;
    args = a;
    i = 0;
    error = false;
}

Symbol classify(String value) {
    if (strcmp(value, "cd") == 0) {
        return SYMBOL_CHANGE_DIRECTORY;
    }

    if (strcmp(value, "exit") == 0) {
        return SYMBOL_EXIT;
    }

    if (strcmp(value, "fg") == 0) {
        return SYMBOL_FOREGROUND;
    }

    if (strcmp(value, "jobs") == 0) {
        return SYMBOL_JOBS;
    }

    if (strcmp(value, "<") == 0) {
        return SYMBOL_READ;
    }

    if (strcmp(value, ">") == 0) {
        return SYMBOL_WRITE;
    }

    if (strcmp(value, ">>") == 0) {
        return SYMBOL_APPEND;
    }

    if (strcmp(value, "|") == 0) {
        return SYMBOL_PIPE;
    }

    for (char* p = invalidChars; *p; p++) {
        if (strchr(value, *p)) {
            return SYMBOL_INVALID;
        }
    }

    return SYMBOL_STRING;
}

void nextsym() {
    if (i >= args->count) {
        sym = SYMBOL_NONE;

        return;
    }

    sym = classify(args->buffer[i]);
    i++;
}

bool accept(Symbol s) {
    if (sym == s) {
        nextsym();

        return true;
    }

    return false;
}

bool expect(Symbol s) {
    if (accept(s)) {
        return true;
    }

    error = true;

    return false;
}

void argument() {
    expect(SYMBOL_STRING);
}

void command_name() {
    expect(SYMBOL_STRING);
}

void command_text() {
    command_name();

    while (sym == SYMBOL_STRING) {
        argument();
    }
}

void file_name() {
    expect(SYMBOL_STRING);
}

void terminate() {
    if (accept(SYMBOL_WRITE) || accept(SYMBOL_APPEND)) {
        expect(SYMBOL_STRING);

        return;
    }

    expect(SYMBOL_NONE);
}

void recursive() {
    expect(SYMBOL_PIPE);
    command_text();

    if (sym == SYMBOL_PIPE) {
        recursive();
    }
    else {
        terminate();
    }

    expect(SYMBOL_NONE);
}

void command() {
    nextsym();

    if (accept(SYMBOL_NONE)) {
        return;
    }

    if (accept(SYMBOL_CHANGE_DIRECTORY)) {
        argument();
        expect(SYMBOL_NONE);

        return;
    }

    if (accept(SYMBOL_EXIT) || accept(SYMBOL_JOBS)) {
        expect(SYMBOL_NONE);

        return;
    }

    if (accept(SYMBOL_FOREGROUND)) {
        argument();
        expect(SYMBOL_NONE);

        return;
    }

    command_text();

    if (accept(SYMBOL_READ)) {
        file_name();

        if (sym == SYMBOL_PIPE) {
            recursive();
        }
        else {
            terminate();
        }

        expect(SYMBOL_NONE);

        return;
    }

    if (sym == SYMBOL_PIPE) {
        recursive();
        expect(SYMBOL_NONE);

        return;
    }

    terminate();

    if (accept(SYMBOL_READ)) {
        file_name();
    }

    expect(SYMBOL_NONE);
}
