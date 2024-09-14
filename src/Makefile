# Makefile
# Copyright (c) 2024 Ishan Pranav
# Licensed under the MIT license.

CC=clang
CFLAGS=-pedantic -std=c99 -Wall -Wextra

all: nyush

nyush: main.c argument_vector handlers string string_builder
	$(CC) $(CFLAGS) argument_vector.o change_directory_handler.o execute_handler.o exit_handler.o string.o string_builder.o main.c -o nyush

handlers: handlers/*.c
	$(CC) $(CFLAGS) -c handlers/*.c

argument_vector: argument_vector.c argument_vector.h
	$(CC) $(CFLAGS) -c argument_vector.c

string: string.c euler.h
	$(CC) $(CFLAGS) -c string.c

string_builder: string_builder.c string_builder.h
	$(CC) $(CFLAGS) -c string_builder.c

clean:
	rm -f *.o nyush
