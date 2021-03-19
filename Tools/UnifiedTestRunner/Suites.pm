package Suites;

use warnings;
use strict;

use Dirs;
use lib Dirs::external_root();

use Module::Pluggable search_path => 'Suites', require => 1, sub_name => 'all_suites';

1;
