#!/bin/sh

set -ex

PROJ_DIR="$(dirname $PWD)"
TARGET="git-stashd"

###
### Clean executable(s) and lib archives.
###

if test -f "$PROJ_DIR/$TARGET"; then
	rm -rf "$PROJ_DIR/$TARGET"
fi

if test -f "$PROJ_DIR/$TARGET.log"; then
	rm -f "$PROJ_DIR/$TARGET.log"
fi

if test -d "$PROJ_DIR/$TARGET.dSYM"; then
	rm -rf "$PROJ_DIR/$TARGET.dSYM"
fi

if test -d "$PROJ_DIR/build"; then
	rm -rf $PROJ_DIR/build/*.o
fi

if test -d "$PROJ_DIR/lib/libgit2"; then
	rm -rf "$PROJ_DIR/lib/libgit2"
fi
