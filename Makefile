###
### Makefile
###

CC      = colorgcc
TARGET  = git-stashd
INSTALL = /usr/bin/install -c

prefix = /usr/local
bindir = $(prefix)/bin
binprefix =

ARFLAGS = rcs
RM      = rm
RMFLAGS = -rf

INCLUDE = include
SOURCES = src
OBJECTS = build

CSFILES = $(wildcard $(SOURCES)/*.c)
OBFILES = $(patsubst %.c,%.o,$(CSFILES))

CFLAGS  = -ggdb -I$(INCLUDE) -I$(SOURCES) -Wall -Wextra
LDFLAGS =

.PHONY: all clean install uninstall

all: $(TARGET)

$(TARGET): $(CSFILES)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

clean:
	$(RM) $(RMFLAGS) $(TARGET) $(TARGET).*

install: all
	$(INSTALL) $(TARGET) $(bindir)/$(TARGET)
	$(RM) $(RMFLAGS) $(TARGET) $(TARGET).*

uninstall:
	$(RM) $(bindir)/$(TARGET)
