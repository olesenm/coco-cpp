#!/bin/sh
#------------------------------------------------------------------------------
cd ${0%/*} || exit 1    # run from this directory

echo "create Parser.cpp and Scanner.cpp for the Coco grammar"
echo

# use Coco in the PWD if possible, otherwise search the PATH
coco=Coco
[ -x "$coco" ] && coco=./Coco

echo "$coco src/Coco-cpp.atg -bak"

$coco src/Coco-cpp.atg -bak
echo

# ----------------------------------------------------------------- end-of-file
