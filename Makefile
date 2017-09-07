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
TOOLS   = tools

CSFILES = $(wildcard $(SOURCES)/*.c)
OBFILES = $(patsubst %.c,%.o,$(CSFILES))

LG2_DIR = lib/libgit2
LG2_ARC = $(LG2_DIR)/build/libgit2.a
LG2_INC = $(LG2_DIR)/include

KERNEL := $(shell sh -c 'uname -s 2>/dev/null || echo unknown')

CFLAGS  = -ggdb -I$(INCLUDE) -I$(LG2_INC) -Wall -Wextra
LDFLAGS = -pthread -lssl -lcrypto -lz

ifeq "$(KERNEL)" "Darwin"
LDFLAGS += -liconv -framework CoreFoundation -framework Security
endif

.PHONY: all build clean install uninstall

all: build $(TARGET)

build:
	@cd $(TOOLS) && ./build.sh

$(TARGET): $(CSFILES)
	$(CC) $(CFLAGS) -o $@ $^ $(LG2_ARC) $(LDFLAGS)

clean:
	$(RM) $(RMFLAGS) $(TARGET) $(TARGET).* $(LG2_DIR)

install:
	@cd $(TOOLS) && ./install.sh

uninstall:
	@cd $(TOOLS) && ./uninstall.sh
