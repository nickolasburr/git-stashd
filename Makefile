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

LG2_DIR = lib/libgit2
LG2_ARC = $(LG2_DIR)/build/libgit2.a
LG2_INC = $(LG2_DIR)/include

CFLAGS  = -ggdb -I$(INCLUDE) -I$(LG2_INC) -Wall -Wextra
LDFLAGS = -pthread -lssl -lcrypto -lz -liconv -framework CoreFoundation -framework Security

.PHONY: all build clean install uninstall

all: build $(TARGET)

build:
	./build.sh

$(TARGET): $(CSFILES)
	$(CC) $(CFLAGS) -o $@ $^ $(LG2_ARC) $(LDFLAGS)

clean:
	$(RM) $(RMFLAGS) $(TARGET) $(TARGET).* $(LG2_DIR)

install: all
	$(INSTALL) $(TARGET) $(bindir)/$(TARGET)
	$(RM) $(RMFLAGS) $(TARGET) $(TARGET).*

uninstall:
	$(RM) $(bindir)/$(TARGET)
