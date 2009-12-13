#!/bin/sh
# compile Coco executable, optionally rebuild the parser itself
#
cd ${0%/*} || exit 1    # run from this directory

[ "$1" = all ] && ./build-parser.sh

echo "compile Coco executable"
echo "~~~~~~~~~~~~~~~~~~~~~~~"
set -x

g++ *.cpp -o Coco -g -Wall

# ----------------------------------------------------------------- end-of-file
