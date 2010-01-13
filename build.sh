#!/bin/sh
#------------------------------------------------------------------------------
usage() {
    while [ "$#" -ge 1 ]; do echo "$1"; shift; done
    cat<<USAGE

usage: ${0##*/} [OPTION]
options:
  -parser        create new scanner/parser code for Coco/R first
  -pedantic      add -pedantic warnings
  -warn          enable additional gcc warnings
  -Wxxx          pass-through gcc -Wxxx directly

* compile coco-cpp executable

USAGE
    exit 1
}
#------------------------------------------------------------------------------
cd ${0%/*} || exit 1    # run from this directory

warn="-Wall"  # warn 'all' is the absolute minimum

# parse options
while [ "$#" -gt 0 ]
do
    case "$1" in
    -h | -help)
        usage
        ;;
    -parser)
        ./build-parser.sh
        ;;
    -pedantic)
        warn="$warn -pedantic"
        ;;
    -warn)
        warn="$warn -Wno-strict-aliasing -Wextra -Wno-unused-parameter -Wold-style-cast -Wnon-virtual-dtor"
        ;;
    -W*)
        warn="$warn $1"
        ;;
    *)
        usage "unknown option/argument: '$*'"
        ;;
    esac
    shift
done


echo "compile Coco executable"
echo "~~~~~~~~~~~~~~~~~~~~~~~"
echo "g++ src/*.cpp -o coco-cpp -g -O2 $warn"
echo

g++ src/*.cpp -o coco-cpp -g -O2 $warn

if [ $? -eq 0 ]
then
    echo
    echo "done"
    echo
else
    echo
    echo "errors detected in compilation"
    echo
fi

# ----------------------------------------------------------------- end-of-file
