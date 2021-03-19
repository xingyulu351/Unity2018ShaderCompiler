use warnings;
use strict;

use Test::More;
use Dirs;

use lib Dirs::external_root();
use Test::MockModule;
use HTTP::Response;

my $OK = new HTTP::Response(200, 'OK');
my $NOT_OK = new HTTP::Response(404, 'NOT_FOUND');
my $userAgentMock = _makeLWPAgentMock();

use_ok('RemoteEventLoggerWebService');

Send_FormsCorrectHttpPostRequest:
{
    my ($uri, %req);
    $userAgentMock->mock(
        post => sub
        {
            (undef, $uri, %req) = @_;
            return $OK;
        }
    );

    _sendDummyRequest();

    is($uri, "uri_value");
    is($req{Content_Type}, "application/json");
    is($req{Content}, '{"type":"dummy_message"}');
}

Send_SuccessfullRequest_Returns0:
{
    $userAgentMock->mock(
        post => sub
        {
            return $OK;
        }
    );
    my $res = _sendDummyRequest();
    is($res, 0);
}

Send_FailedRequest_Returns1AndLogsWarning:
{
    my $loggerMock = new Test::MockModule('Logger');
    my $logWarningInvoked = 0;
    $loggerMock->mock(
        warning => sub
        {
            $logWarningInvoked = 1;
        }
    );
    $userAgentMock->mock(
        post => sub
        {
            return $NOT_OK;
        }
    );
    my $res = _sendDummyRequest();
    is($res, 1);
    is($logWarningInvoked, 1);
}

Send_ReturnsContent:
{
    $userAgentMock->mock(
        post => sub
        {
            my $response = new HTTP::Response();
            $response->code(200);
            $response->header(Content_Type => 'application/json');
            $response->content('{"foo":"bar"}');
            return $response;
        }
    );

    my ($result, $content) = _sendDummyRequest();
    is_deeply($content, { foo => 'bar' });
}

done_testing();

sub _sendDummyRequest
{
    my $dummyRequest = { 'type' => 'dummy_message' };
    return RemoteEventLoggerWebService::send('uri_value', $dummyRequest);
}

sub _makeLWPAgentMock
{
    my $userAgentMock = new Test::MockModule('LWP::UserAgent');
    return $userAgentMock;
}
