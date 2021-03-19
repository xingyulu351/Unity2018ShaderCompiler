package TimeUtils;

use warnings;
use strict;

use Time::HiRes 'gettimeofday';
use Time::Local 'timelocal';

sub getUTCNowMs
{
    my $time = int(gettimeofday * 1000);
    my $secOffset = timelocal(localtime()) - timelocal(gmtime());
    $time -= $secOffset * 1000;
    return $time;
}

1;
