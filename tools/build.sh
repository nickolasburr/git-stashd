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
LG2_TAG="v1.9.2"

if test ! -d "$LG2_DIR"; then
	$GIT -c advice.detachedHead=false -C "$LIB_DIR" clone --depth=1 --branch="$LG2_TAG" --quiet "$LG2_REPO"
fi

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
