# Makefile
# Copyright (c) 2024 Ishan Pranav
# Licensed under the MIT license.

# References:
#  - https://www.man7.org/linux/man-pages/man3/getline.3.html
#  - https://www.man7.org/linux/man-pages/man3/strdup.3.html

# getline in <stdio.h>: _POSIX_C_SOURCE >= 200809L
# strdup in <string.h>: _XOPEN_SOURCE >= 500

CC=clang
CFLAGS=-D_XOPEN_SOURCE=500 -D_POSIX_C_SOURCE=200809L -pedantic -std=c99 -Wall -Wextra

all: nyush

nyush: main.c argument_vector handlers job_collection parser
	$(CC) $(CFLAGS) *.o main.c -o nyush

argument_vector: argument_vector.c argument_vector.h
	$(CC) $(CFLAGS) -c argument_vector.c

handlers: *_handler.c handler.h
	$(CC) $(CFLAGS) -c *_handler.c

job_collection: job_collection.c job_collection.h
	$(CC) $(CFLAGS) -c job_collection.c

parser: parser.c parser.h
	$(CC) $(CFLAGS) -c parser.c

clean:
	rm -f *.o nyush
