START_DIR=`pwd`
SCRIPT_DIR=`dirname $0`
GENERATOR=$1
cd $SCRIPT_DIR
../../External/Jamplus/builds/bin/macosx32/jam --workspace --config=macx.config --gen=$GENERATOR Jamfile.jam workspace
cd $START_DIR
