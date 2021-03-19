package Sleep;

use Time::HiRes;

sub sleep
{
    my ($interval) = @_;
    Time::HiRes::sleep($interval);
}

1;
