#!/bin/sh

die() {
    echo "$1";
    exit 1;
}

python setup.py clean --all
python setup.py build --pygibson-debug || die "build failed"

PYTHON_VER="`python -c 'import sys; print sys.version[:3]'`"
PLATFORM="`python -c 'from distutils import util; print util.get_platform();'`"
BUILD_DIR="build/lib.$PLATFORM-$PYTHON_VER"

echo
echo ----------------------------------------------------------------------
PYTHONPATH=$BUILD_DIR python -m tests.run
