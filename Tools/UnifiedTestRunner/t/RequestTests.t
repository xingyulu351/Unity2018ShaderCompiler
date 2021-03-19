use warnings;
use strict;

use Test::More;

use Dirs;
use lib Dirs::external_root();
use Test::MockModule;

use HTTP::Request::Common;

BEGIN { use_ok('Request'); }

can_ok('Request', 'send');

my $OK = new HTTP::Response(200, 'OK');
my $NOT_OK = new HTTP::Response(404, 'NOT_FOUND');

my $sleepMock = new Test::MockModule('Sleep');
$sleepMock->mock(sleep => sub { });

send_SendsProperHttpRequest:
{
    my $lwpMock = new Test::MockModule('LWP::UserAgent');
    my $capturedRequest;
    $lwpMock->mock(
        request => sub
        {
            (undef, $capturedRequest) = @_;
            return $OK;
        }
    );

    my $r = _makeRequest();
    my $response = Request::send(request => $r);

    is_deeply($capturedRequest, $r);
    isa_ok($response, 'HTTP::Response');
}

send_PrintsAnErrorIfRequestFails:
{
    my $lwpMock = new Test::MockModule('LWP::UserAgent');
    my $capturedRequest;
    $lwpMock->mock(
        request => sub
        {
            return $NOT_OK;
        }
    );
    my $loggerMock = new Test::MockModule('Logger');
    my ($loggedText) = @_;
    $loggerMock->mock(
        error => sub
        {
            ($loggedText) = @_;
        }
    );

    Request::send(request => _makeRequest());
    like($loggedText, qr/NOT_FOUND/);
}

Send_DoesExactlyOneAttemptIfRetriesCountIsNotSpecified:
{
    my $attemptsCount = 0;
    my $lwpMock = new Test::MockModule('LWP::UserAgent');
    $lwpMock->mock(
        request => sub
        {
            ++$attemptsCount;
            return $NOT_OK;
        }
    );

    Request::send(request => _makeRequest());

    is($attemptsCount, 1);
}

Send_DoesSpecifiedNumberOfAttemptsBeforeFails:
{
    my $attemptsCount = 0;
    my $lwpMock = new Test::MockModule('LWP::UserAgent');
    $lwpMock->mock(
        request => sub
        {
            ++$attemptsCount;
            return $NOT_OK;
        }
    );

    Request::send(request => _makeRequest(), retries => 3);

    is($attemptsCount, 4);
}

Send_RetiresUtilFirstSuccess:
{
    my $attemptsCount = 0;
    my $lwpMock = new Test::MockModule('LWP::UserAgent');
    $lwpMock->mock(
        request => sub
        {
            if (++$attemptsCount == 5)
            {
                return $OK;
            }
            return $NOT_OK;
        }
    );

    Request::send(
        request => _makeRequest(),
        retries => 10
    );

    is($attemptsCount, 5);
}

Send_SleepsForASpecifiedIntervalBeforeAttemts:
{
    my $lwpMock = new Test::MockModule('LWP::UserAgent');
    $lwpMock->mock(
        request => sub
        {
            return $NOT_OK;
        }
    );

    my $sleepMock = new Test::MockModule('Sleep');
    my $sleepsCount = 0;
    my $totalSleepTime = 0;
    $sleepMock->mock(
        sleep => sub
        {
            my ($interval) = @_;
            ++$sleepsCount;
            $totalSleepTime += $interval;
        }
    );

    Request::send(
        request => _makeRequest(),
        retries => 2,
        retryAfterSec => 10,
    );

    is($sleepsCount, 3);
    is($totalSleepTime, 3 * 10);
}

Send_DefaultRetryAfterSecValueIs5Secs:
{
    my $lwpMock = new Test::MockModule('LWP::UserAgent');
    $lwpMock->mock(
        request => sub
        {
            return $NOT_OK;
        }
    );
    my $sleepMock = new Test::MockModule('Sleep');
    my $sleepsCount = 0;
    my $totalSleepTime = 0;
    $sleepMock->mock(
        sleep => sub
        {
            my ($interval) = @_;
            ++$sleepsCount;
            $totalSleepTime += $interval;
        }
    );

    Request::send(
        request => _makeRequest(),
        retries => 2,
    );

    is($sleepsCount, 3);
    is($totalSleepTime, 3 * 5);
}

done_testing();

sub _makeRequest
{
    my $r = HTTP::Request->new(GET => 'http://www.example.com/',);
    return $r;
}
