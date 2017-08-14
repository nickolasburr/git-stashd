###
### Makefile
###

CC      = gcc
TARGET  = git-stashd
PATH    = $(shell printenv PATH)

ARFLAGS = rcs
RM      = rm
RMFLAGS = -rf

INCLUDE = include
SOURCES = src
OBJECTS = build

CSFILES = $(wildcard $(SOURCES)/*.c)
OBFILES = $(patsubst %.c,%.o,$(CSFILES))

CFLAGS  = -ggdb -I$(INCLUDE) -I$(SOURCES) -Wall -Wextra
LDFLAGS=

.PHONY: all build clean

all: $(TARGET)

$(TARGET): $(CSFILES)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

clean:
	$(RM) $(RMFLAGS) $(OBJECTS)/*.o $(SOURCES)/*.o $(TARGET) $(TARGET).log $(TARGET).dSYM
