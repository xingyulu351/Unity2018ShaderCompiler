use strict;
use warnings;
use PatchQtLibs qw (PatchQtLibs);

# first arg is a path to root Qt directory that has to be patched,
# e.g. /Users/dmitryo/code/unity2/External/Qt/mac/macx32/builds or ../../External/Qt/mac/macx32/builds
# also there is should be a 'lib' subdir, e.g. ../../External/Qt/mac/macx32/builds/libs

if (!defined $ARGV[0])
{
    croak("Please specify root Qt dir to be patched as a first arg\n");
}

PatchQtLibs($ARGV[0], 0);

1;
