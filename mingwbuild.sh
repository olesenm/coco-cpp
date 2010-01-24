#!/bin/sh
# cross-compile coco-cpp for windows using mingw32
# be pedantic about warnings

cd ${0%/*} || exit 1    # run from this directory
set -x

/opt/cross/bin/i386-mingw32msvc-g++ \
    src/*.cpp -o coco-cpp.exe -g -O2 -Wl,-subsystem,console \
    -Wall -Wno-strict-aliasing -Wextra -Wno-unused-parameter \
    -Wold-style-cast -Wnon-virtual-dtor \
    -pedantic

# ----------------------------------------------------------------- end-of-file
