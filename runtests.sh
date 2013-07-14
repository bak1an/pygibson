#!/bin/sh

die() {
    echo "$1";
    exit 1;
}

python setup.py clean --all
python setup.py build --pygibson-debug || die "build failed"

PYTHON_VER="`python -c 'import sys; print sys.version[:3]'`"
BUILD_DIR="build/lib.linux-`uname -p`-$PYTHON_VER"  ## only linux is here, i know

echo
echo ----------------------------------------------------------------------

PYTHONPATH=$BUILD_DIR python -m tests.run
