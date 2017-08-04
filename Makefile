###
### Makefile
###
### Copyright (C) 2017 Nickolas Burr
###

CC=gcc
INCDIR=include
CFLAGS=-c -Wall -I$(INCDIR)
LDFLAGS=
SOURCES=src/daemon.c src/main.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=git-stashd

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

daemon.o:
	$(CC) $(CFLAGS) $< -o $@
