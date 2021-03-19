#!/usr/bin/env perl
use warnings;
use strict;

use FindBin qw ($Bin);
use lib "$FindBin::Bin/../../";

use Dirs;
use lib Dirs::external_root();
use Readonly;
use Mojolicious::Lite;

my $clients = {};

get '/' => { text => 'Unified Test Runner REST API' };
