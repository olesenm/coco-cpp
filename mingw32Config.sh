#!/bin/sh
# configure for cross-compiling coco-cpp using mingw32
cd ${0%/*} || exit 1    # run from this directory
set -x

[ -f Makefile ] && make distclean

./configure \
    --host=i386-mingw32msvc \
    CXXFLAGS="-g -O2 -Wl,-subsystem,console -Wall -Wno-strict-aliasing -Wextra -Wno-unused-parameter -Wold-style-cast -Wnon-virtual-dtor -pedantic" \
    $@

# ----------------------------------------------------------------- end-of-file
