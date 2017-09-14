#!/bin/sh

set -ex

PROJ_DIR="$(dirname $PWD)"
BUILD_DIR="$PROJ_DIR/build"
LIB_DIR="$PROJ_DIR/lib"
LG2_DIR="$LIB_DIR/libgit2"
LG2_BUILD_DIR="$LG2_DIR/build"
LG2_REPO="https://github.com/libgit2/libgit2.git"
LG2_SHA="15e119375018fba121cf58e02a9f17fe22df0df8"

if test ! -d "$LG2_DIR"; then
	git -C "$LIB_DIR" clone --quiet "$LG2_REPO"
fi

cd $LG2_DIR && git checkout --quiet "$LG2_SHA"

if test ! -d $LG2_BUILD_DIR; then
	mkdir -p $LG2_BUILD_DIR
fi

cd $LG2_BUILD_DIR &&

cmake -DTHREADSAFE=ON                                                                    \
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

cmake --build .
