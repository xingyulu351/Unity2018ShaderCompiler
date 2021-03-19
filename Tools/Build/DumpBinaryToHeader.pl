#!/usr/bin/perl

use warnings;
use strict;

use File::Basename qw (dirname basename fileparse);
use File::Path;
use Cwd qw (realpath);
use FindBin qw ($Bin);
use lib ("$Bin");
use Tools qw (DumpBinaryToByteArray);

if (@ARGV != 1)
{
    die("Usage: DumpBinaryToHeader.pl myBinary");
}

DumpBinaryToByteArray($ARGV[0]);

1;
