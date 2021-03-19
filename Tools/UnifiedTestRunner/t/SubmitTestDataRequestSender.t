use warnings;
use strict;

use Dirs;
use lib Dirs::external_root();

use Test::More;
use Test::MockModule;

my $OK = new HTTP::Response(200, 'OK');
my $NOT_OK = new HTTP::Response(404, 'NOT_FOUND');

BEGIN { use_ok('SubmitTestDataRequestSender'); }

can_ok('SubmitTestDataRequestSender', 'send');

HttpPostRequestIsStructuredProperly:
{
    my ($uri, @args);

    my $userAgentMock = makeLWPAgentMock();
    $userAgentMock->mock(
        post => sub
        {
            shift();
            ($uri, @args) = @_;
            return $OK;
        }
    );

    my $returnValue = SubmitTestDataRequestSender::send(
        uri => 'http://localhost',
        header => 'a',
        files => ['tr1.xml']
    );

    is($returnValue, 1);
    is($uri, "http://localhost");
    my %a = @args;

    is($uri, "http://localhost");
    is($a{Content_Type}, "form-data");

    my %content = @{ $a{Content} };
    is($content{request}, 'a');
    is(@{ $content{'tr1.xml'} }[0], 'tr1.xml');
}

HttpPostRequestIsStructuredProperlyWhen2ReferencedFilesAreGived:
{
    my ($uri, @args);

    my $userAgentMock = makeLWPAgentMock();
    $userAgentMock->mock(
        post => sub
        {
            shift();
            ($uri, @args) = @_;
            return $OK;
        }
    );

    my $returnValue = SubmitTestDataRequestSender::send(
        uri => 'http://localhost',
        header => 'a',
        files => ['tr1.xml', 'tr2.xml']
    );

    is($returnValue, 1);
    is($uri, "http://localhost");
    my %a = @args;

    is($uri, "http://localhost");
    is($a{Content_Type}, "form-data");

    my %content = @{ $a{Content} };
    is($content{request}, 'a');

    is(@{ $content{'tr1.xml'} }[0], 'tr1.xml');
    is(@{ $content{'tr2.xml'} }[0], 'tr2.xml');
}

NoReferencesFilesSetContentTypeToJson:
{
    my ($uri, @args);

    my $userAgentMock = makeLWPAgentMock();
    $userAgentMock->mock(
        post => sub
        {
            shift();
            ($uri, @args) = @_;
            return $OK;
        }
    );

    my $returnValue = SubmitTestDataRequestSender::send(
        uri => 'http://localhost',
        header => '{"data":1}',
        files => []
    );

    is($returnValue, 1);
    my %a = @args;
    is($uri, "http://localhost");
    is($a{Content_Type}, "application/json");
    is($a{Content}, '{"data":1}');
}

Send_ReturnsZeroIfHttpRequestWasNotOk:
{
    my $userAgentMock = makeLWPAgentMock();
    $userAgentMock->mock(post => sub { return $NOT_OK; });
    my $returnValue = SubmitTestDataRequestSender::send();
    is($returnValue, 0);
}

Send_DoesExactlyOneAttemptIfRetriesCountIsNotSpecified:
{
    my $attemptsCount = 0;
    my $userAgentMock = makeLWPAgentMock();

    $userAgentMock->mock(
        post => sub
        {
            ++$attemptsCount;
            return $NOT_OK;
        }
    );

    my $returnValue = SubmitTestDataRequestSender::send(
        uri => 'http://localhost',
        header => 'a',
        file => 'tr1.xml'
    );
    is($returnValue, 0);
    is($attemptsCount, 1);
}

Send_DoesSpecifiedNumberOfAttempts:
{
    my $attemptsCount = 0;
    my $userAgentMock = makeLWPAgentMock();

    $userAgentMock->mock(
        post => sub
        {
            ++$attemptsCount;
            return $NOT_OK;
        }
    );

    my $returnValue = SubmitTestDataRequestSender::send(
        uri => 'http://localhost',
        header => 'a',
        file => 'tr1.xml',
        retries => 3
    );

    is($returnValue, 0);
    is($attemptsCount, 3);
}

Send_RetiresUtilFirstSuccess:
{
    my $attemptsCount = 0;

    my $userAgentMock = makeLWPAgentMock();

    $userAgentMock->mock(
        post => sub
        {
            if (++$attemptsCount == 5)
            {
                return $OK;
            }
            return $NOT_OK;
        }
    );

    my $returnValue = SubmitTestDataRequestSender::send(
        uri => 'http://localhost',
        header => 'a',
        file => 'tr1.xml',
        retries => 10
    );

    is($attemptsCount, 5);
}

done_testing();

sub makeLWPAgentMock
{
    my $userAgentMock = new Test::MockModule('LWP::UserAgent');
    return $userAgentMock;
}
