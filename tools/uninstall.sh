#!/bin/sh

set -ex

PREFIX=/usr/local
BINDIR=$PREFIX/bin

RM="rm"
RMFLAGS="-rf"
TARGET="git-stashd"

eval "$RM $RMFLAGS $BINDIR/$TARGET"
