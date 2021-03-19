package OSUtils;

use warnings;
use strict;

sub isWindows
{
    return $^O =~ /mswin32|cygwin|msys/i;
}

1;
