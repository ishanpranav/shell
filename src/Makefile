# Makefile
# Copyright (c) 2024 Ishan Pranav
# Licensed under the MIT license.

CC=clang
CFLAGS=-pedantic -std=c99 -Wall -Werror -Wextra

all: nyush

nyush: main.o

main.o: main.c

clean:
	rm -f *.o nyush
