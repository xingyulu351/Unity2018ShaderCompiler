
use warnings;
use strict;
use File::Basename qw (dirname basename fileparse);
use File::Spec;
use lib File::Spec->rel2abs(dirname(__FILE__));
use Tools qw ( GenerateUnityEvents );

my $root = File::Spec->rel2abs(dirname(__FILE__)) . '/../..';

GenerateUnityEvents($ARGV[0], $ARGV[1], $ARGV[2]);
