package Commands;

use warnings;
use strict;

sub getUtrPrefix
{
    if ($^O eq 'MSWin32')
    {
        return "perl utr.pl";
    }
    return "./utr.pl";
}

1;
