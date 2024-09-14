// parser.c
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "euler.h"
#include "parser.h"
#include "symbol.h"

Symbol sym;
ArgumentVector args;
size_t i;

void parser(ArgumentVector a) {
    sym = SYMBOL_NONE;
    args = a;
    i = 0;
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

void error(String message) {
    fprintf(stderr, "%s", message);
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

    error("expect: unexpected symbol\n");

    return false;
}

void argument() {
    expect(SYMBOL_STRING);
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

    if (accept(SYMBOL_EXIT)) {
        expect(SYMBOL_NONE);

        return;
    }
}
