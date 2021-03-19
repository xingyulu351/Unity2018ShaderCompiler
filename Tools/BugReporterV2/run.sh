START_DIR=`pwd`
SCRIPT_DIR=`dirname $0`
cd $SCRIPT_DIR
if ./build.sh -sCONFIG=release bugreporter; then
    open ../../build/MacEditor/Unity.app/Contents/BugReporter/Unity\ Bug\ Reporter.app $@
fi
cd $START_DIR
