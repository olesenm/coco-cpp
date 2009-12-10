#!/bin/sh
echo "compile Coco executable"
echo
set -x

g++ *.cpp -o Coco -g -Wall
