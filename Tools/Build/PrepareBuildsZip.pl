#!/usr/bin/env perl

use warnings;
use strict;

use FindBin qw($Bin);
use lib ("$Bin");
use PrepareWorkingCopy qw(UnzipExternalDependency);

if (@ARGV == 1)
{
    UnzipExternalDependency($ARGV[0]);
}
elsif (@ARGV == 2)
{
    UnzipExternalDependency($ARGV[0], $ARGV[1]);
}
elsif (@ARGV == 3)
{
    UnzipExternalDependency($ARGV[0], $ARGV[1], $ARGV[2]);
}
elsif (@ARGV == 4)
{
    UnzipExternalDependency($ARGV[0], $ARGV[1], $ARGV[2], $ARGV[3]);
}
else
{
    die "Bad arguments in prepare builds zip";
}

1;
