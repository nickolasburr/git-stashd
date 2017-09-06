#!/usr/bin/env sh

set -ex

PREFIX=/usr/local
BINDIR=$PREFIX/bin

INSTALL=/usr/bin/install
OPTIONS="-c"
TARGET="git-stashd"

cd .. && eval "$INSTALL $OPTIONS $TARGET $BINDIR/$TARGET"
