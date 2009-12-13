#!/bin/sh
cd ${0%/*} || exit 1    # run from this directory

echo "recreate Parser.cpp and Scanner.cpp for the Coco grammar"
echo

# use Coco in the PWD if possible, otherwise search the PATH
coco=Coco
[ -x "$coco" ] && coco=./Coco

echo "$coco Coco.atg -bak -namespace Coco"

$coco Coco.atg -bak -namespace Coco
echo

# ----------------------------------------------------------------- end-of-file
