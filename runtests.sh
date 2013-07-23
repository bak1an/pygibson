#!/bin/sh

die() {
    echo "$1";
    exit 1;
}

DEBUG="false";
if [ "$1x" = "--debugx" ]; then  # too lazy to getopt now
    DEBUG="true";
    export CFLAGS="$CFLAGS -O0 -g";
fi;

python setup.py clean --all
python setup.py build --pygibson-debug || die "build failed"

PYTHON_VER="`python -c 'import sys; print sys.version[:3]'`"
PLATFORM="`python -c 'from distutils import util; print util.get_platform();'`"
BUILD_DIR="build/lib.$PLATFORM-$PYTHON_VER"

echo
echo ----------------------------------------------------------------------
if [ "$DEBUG" = "false" ]; then
    PYTHONPATH=$BUILD_DIR python -m tests.run > /tmp/debug_out;
    echo "and here is your debug from _pygibson:";
    cat /tmp/debug_out;
else
    PYTHONPATH=$BUILD_DIR gdb python;
fi;
