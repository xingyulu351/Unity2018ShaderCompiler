package Request;

use warnings;
use strict;

use Sleep;

use Dirs;
use lib Dirs::external_root();

use JSON;
use LWP::UserAgent;

sub send
{
    my (%args) = @_;
    my $ua = new LWP::UserAgent();
    my $retries = $args{retries};
    if (not $retries)
    {
        $retries = 0;
    }
    my $retryAfterSec = $args{retryAfterSec};
    if (not $retryAfterSec)
    {
        $retryAfterSec = 5;
    }
    my $response;
    for (my $i = 0; $i <= $retries; $i++)
    {
        $response = $ua->request($args{request});
        if ($response->is_error())
        {
            Logger::error("Can not make http request: " . $response->as_string());
            Sleep::sleep($retryAfterSec);
            next;
        }
        last;
    }
    return $response;
}

1;
