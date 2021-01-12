CC = gcc
CFLAGS = -pthread -g -Wall -ansi -pedantic -D_POSIX_C_SOURCE=199309L
LD = gcc
LDFLAGS =
all: mush
mush: main.c
	$(CC) $(CFLAGS) -o threads main.c
