# Makefile
# Copyright (c) 2024 Ishan Pranav
# Licensed under the MIT license.

CC=clang
CFLAGS=-pedantic -std=c99 -Wall -Wextra

all: nyush

nyush: main.c string_builder
	$(CC) $(CFLAGS) string_builder.o main.c -o nyush

string_builder: string_builder.c string_builder.h
	$(CC) $(CFLAGS) -c string_builder.c

clean:
	rm -f *.o nyush
