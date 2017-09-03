#!/usr/bin/env sh

set -ex

BUILD_DIR="$PWD/build"
LIB_DIR="$PWD/lib"
LG2_DIR="$LIB_DIR/libgit2"
LG2_BUILD_DIR="$LG2_DIR/build"
LG2_REPO="https://github.com/libgit2/libgit2.git"
LG2_SHA="15e119375018fba121cf58e02a9f17fe22df0df8"

if test ! -d "$LG2_DIR"; then
	git -C "$LIB_DIR" clone "$LG2_REPO"
fi

cd $LG2_DIR && git checkout "$LG2_SHA"

if test ! -d $LG2_BUILD_DIR; then
	mkdir -p $LG2_BUILD_DIR
fi

cd $LG2_BUILD_DIR &&

cmake -DTHREADSAFE=ON                     \
      -DBUILD_CLAR=OFF                    \
      -DBUILD_SHARED_LIBS=OFF             \
      -DCMAKE_C_FLAGS=-fPIC               \
      -DCMAKE_BUILD_TYPE="RelWithDebInfo" \
      -DCMAKE_INSTALL_PREFIX="$BUILD_DIR" \
      .. &&

cmake --build .
