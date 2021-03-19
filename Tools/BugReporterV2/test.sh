#!/bin/sh
START_DIR=`pwd`
SCRIPT_DIR=`dirname $0`
cd $SCRIPT_DIR
./build.sh all test $@
cd $START_DIR
