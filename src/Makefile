# Makefile
# Copyright (c) 2024 Ishan Pranav
# Licensed under the MIT license.

CC=clang
CFLAGS=-pedantic -std=c99 -Wall -Wextra

all: nyush

nyush: main.c argument_vector handlers parser string string_builder
	$(CC) $(CFLAGS) exit_handler.o change_directory_handler.o execute_handler.o argument_vector.o parser.o string.o string_builder.o main.c -o nyush

argument_vector: argument_vector.c argument_vector.h
	$(CC) $(CFLAGS) -c argument_vector.c

handlers: handlers/*.c
	$(CC) $(CFLAGS) -c handlers/*.c

parser: parser.c parser.h
	$(CC) $(CFLAGS) -c parser.c

string: string.c euler.h
	$(CC) $(CFLAGS) -c string.c

string_builder: string_builder.c string_builder.h
	$(CC) $(CFLAGS) -c string_builder.c

clean:
	rm -f *.o nyush
