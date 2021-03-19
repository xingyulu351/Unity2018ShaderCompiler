#!/usr/bin/env perl

use warnings;
use strict;

use File::Basename qw(dirname basename fileparse);
use File::Path;
use Cwd qw(realpath);
use FindBin qw($Bin);
use lib ("$Bin");
use DownloadAndUnpackArchive qw(DownloadAndUnpackArchive);

if (@ARGV != 4)
{
    die "Bad arguments in prepare builds zip";
}

DownloadAndUnpackArchive($ARGV[0], $ARGV[1], $ARGV[2], $ARGV[3]);

1;
