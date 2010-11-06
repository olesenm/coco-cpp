#!/bin/sh
#------------------------------------------------------------------------------
cd ${0%/*} || exit 1    # run from this directory

echo "create Parser.cpp and Scanner.cpp for the Coco grammar"
echo

# use Coco from src/ if possible, otherwise search the PATH
coco=coco-cpp
[ -x "src/$coco" ] && coco=src/coco-cpp

echo "$coco src/Coco-cpp.atg -bak"

$coco src/Coco-cpp.atg -bak
echo

# ----------------------------------------------------------------- end-of-file
