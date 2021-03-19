#!/bin/sh
START_DIR=`pwd`
SCRIPT_DIR=`dirname $0`
cd $SCRIPT_DIR
if ./build.sh -sCONFIG=debug bugreporter.test_ui; then
    ./build/macosx32-debug/test_ui/app/test_ui.debug $@
fi
cd $START_DIR
