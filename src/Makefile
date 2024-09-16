# Makefile
# Copyright (c) 2024 Ishan Pranav
# Licensed under the MIT license.

CC=gcc
CFLAGS=-pedantic -std=c99 -Wall -Wextra

all: nyush

nyush: main.c shell.h argument_vector handlers parser string string_builder
	$(CC) $(CFLAGS) *.o main.c -o nyush

argument_vector: argument_vector.c argument_vector.h
	$(CC) $(CFLAGS) -c argument_vector.c

handlers: *_handler.c handler.h
	$(CC) $(CFLAGS) -c *_handler.c

parser: parser.c parser.h
	$(CC) $(CFLAGS) -c parser.c

string: string.c euler.h
	$(CC) $(CFLAGS) -c string.c

string_builder: string_builder.c string_builder.h
	$(CC) $(CFLAGS) -c string_builder.c

clean:
	rm -f *.o nyush
