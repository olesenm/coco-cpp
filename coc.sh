#!/bin/sh
echo "recreate Parser.cpp and Scanner.cpp for the Coco grammar"
echo
set -x

./Coco Coco.atg -namespace Coco
