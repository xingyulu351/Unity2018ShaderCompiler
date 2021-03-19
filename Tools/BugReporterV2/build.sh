#!/bin/sh
START_DIR=`pwd`
SCRIPT_DIR=`dirname $0`
cd $SCRIPT_DIR
../../External/Jamplus/builds/bin/macosx32/jam $@
RESULT=$?
cd $START_DIR
exit $RESULT
