#!/bin/sh

set -ex

WHICH="$(\which which)"
CMAKE="$($WHICH cmake)"
GIT="$($WHICH git)"
MKDIR="$($WHICH mkdir)"

PROJ_DIR="$(dirname $PWD)"
BUILD_DIR="$PROJ_DIR/build"
LIB_DIR="$PROJ_DIR/lib"
LG2_DIR="$LIB_DIR/libgit2"
LG2_BUILD_DIR="$LG2_DIR/build"
LG2_REPO="https://github.com/libgit2/libgit2.git"
LG2_SHA="338e6fb681369ff0537719095e22ce9dc602dbf0"

if test ! -d "$LG2_DIR"; then
	$GIT -C "$LIB_DIR" clone --quiet "$LG2_REPO"
fi

cd $LG2_DIR && $GIT checkout --quiet "$LG2_SHA"

if test ! -d $LG2_BUILD_DIR; then
	$MKDIR -p $LG2_BUILD_DIR
fi

cd $LG2_BUILD_DIR &&

$CMAKE -DTHREADSAFE=ON                                                                    \
       -DBUILD_CLAR=OFF                                                                   \
       -DBUILD_SHARED_LIBS=OFF                                                            \
       -DUSE_SSH=OFF                                                                      \
       -DCURL=OFF                                                                         \
       -DCMAKE_C_FLAGS=-fPIC                                                              \
       -DCMAKE_C_ARCHIVE_CREATE="<CMAKE_AR> Scr <TARGET> <LINK_FLAGS> <OBJECTS>"          \
       -DCMAKE_C_ARCHIVE_FINISH="<CMAKE_RANLIB> -no_warning_for_no_symbols -c <TARGET>"   \
       -DCMAKE_BUILD_TYPE="RelWithDebInfo"                                                \
       -DCMAKE_INSTALL_PREFIX="$BUILD_DIR"                                                \
       .. &&

$CMAKE --build .
