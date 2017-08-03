###
### Makefile
###
### Copyright (C) 2017 Nickolas Burr
###

INCDIR=include
CC=gcc
CFLAGS=-I$(INCDIF)

OBJDIR=obj
LIBDIR=lib

LIBS=-lm

_DEPS = common.h daemon.h
DEPS = $(patsubst %,$(INCDIR)/%,$(_DEPS))

_OBJ = daemon.c main.c
OBJ = $(patsubst %,$(OBJDIR)/%,$(_OBJ))

$(OBJDIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

git-stashd: $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(OBJDIR)/*.o *~ core $(INCDIR)/*~
