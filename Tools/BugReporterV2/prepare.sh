START_DIR=`pwd`
SCRIPT_DIR=`dirname $0`
cd $SCRIPT_DIR
../../jam Qt $@
cd $START_DIR
