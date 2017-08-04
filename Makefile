###
### Makefile
###
### Copyright (C) 2017 Nickolas Burr
###

CC=gcc
INCDIR=include
SRCDIR=src
CFLAGS=-c -Wall -I$(INCDIR)
LDFLAGS=
SOURCES=$(SRCDIR)/daemon.c $(SRCDIR)/usage.c $(SRCDIR)/main.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=git-stashd

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.PHONY: clean

clean:
	rm -f $(SRCDIR)/*.o
