#!/usr/bin/perl

use warnings;
use strict;

use FindBin qw ($Bin);
use lib $Bin;
use Dirs;
use lib Dirs::external_root();
use File::pushd;
use Logger;
use SystemCall;

my $newDir = pushd($Bin);

Logger::initialize('minimal');

my $system = new SystemCall();

my $exitCode = $system->execute('prove', '-r');

if ($exitCode != 0)
{
    Logger::error("One or more UTR tests has failed. Test process exited with $exitCode");
}

exit($exitCode);
