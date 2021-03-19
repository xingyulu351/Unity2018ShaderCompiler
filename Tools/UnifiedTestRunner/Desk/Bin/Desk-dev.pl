#!/usr/bin/env perl

use FindBin qw ($Bin);
use lib "$Bin/../..";
use Dirs;

my $externalRoot = Dirs::external_root();

system("perl", "-I$externalRoot", "$externalRoot/Mojolicious/bin/morbo", "$Bin/../Desk/Desk.pl");
