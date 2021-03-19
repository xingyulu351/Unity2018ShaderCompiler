use warnings;
use strict;

use Test::More;

use Dirs;
use lib Dirs::external_root();

use Test::MockModule;

BEGIN { use_ok('RemoteEventLogger'); }

my $wsMock = new Test::MockModule('RemoteEventLoggerWebService');

initialize_FetchesFiltersFromRemoteUrl:
{
    my $unityTestProtocolMock = new Test::MockModule('UnityTestProtocol::Messages');
    $unityTestProtocolMock->mock(makeSessionStartMessage => sub { return { data => { 'a' => 'b' } }; });

    my $uri;
    my $req;
    $wsMock->mock(
        send => sub
        {
            ($uri, $req) = @_;
            return 0;
        }
    );

    RemoteEventLogger::initialize(
        owner => 'owner_value',
        uri => 'uri_value',
    );

    is($uri, "uri_value/sessions/init");
    is_deeply($req, { 'a' => 'b' });
}

initialize_LogsWarningIsFailedToFetchRemoteSettings:
{
    $wsMock->mock(
        send => sub
        {
            return 1;    #failed
        }
    );

    my $warningInvoked;
    my $loggerMock = new Test::MockModule('Logger');
    $loggerMock->mock(
        warning => sub
        {
            $warningInvoked = 1;
        }
    );

    RemoteEventLogger::initialize(
        owner => 'owner_value',
        uri => 'uri_value',
    );

    is($warningInvoked, 1);
}

log_LogMessageFailed_LogsWarning:
{
    my $loggerMock = new Test::MockModule('Logger');
    $wsMock->mock(
        send => sub
        {
            return 1;
        }
    );

    _init(
        remoteSettings => {
            filters => [{ messageType => 't1' }]
        }
    );

    my $invoked = 0;
    $loggerMock->mock(
        warning => sub
        {
            $invoked = 1;
        }
    );

    is($invoked, 0);

    logMsg({ type => 't1' });

    is($invoked, 1);
}

initialize_NoOnwer_DoesNotRequests:
{
    my $invokesCount = 0;
    $wsMock->mock(
        send => sub
        {
            ++$invokesCount;
            return 0;
        }
    );

    RemoteEventLogger::initialize(
        uri => 'uri_value',
        owner => undef,
    );

    is($invokesCount, 0);
}

initialize_NoUri_DoesNotSendAnyRequests:
{
    my $invokesCount = 0;
    $wsMock->mock(
        send => sub
        {
            ++$invokesCount;
            return 0;
        }
    );

    RemoteEventLogger::initialize(
        uri => undef,
        owner => 'owner',
    );

    logMsg({ type => 't1' });

    is($invokesCount, 0);
}

log_WrappsMessageCorrectly:
{
    my $timeUtilsMock = new Test::MockModule('TimeUtils');
    $timeUtilsMock->mock(getUTCNowMs => sub { return 42; });

    my ($uri, $req);
    $wsMock->mock(
        send => sub
        {
            ($uri, $req) = @_;
        }
    );

    _init(
        localSetting => {
            owner => 'owner_value',
            uri => 'uri_value',
        },
        remoteSettings => {
            sendMessageIntervalSec => 0,
            sessionId => 1,
            filters => [{ messageType => 't1' }]
        }
    );

    logMsg({ type => 't1' });

    is($uri, 'uri_value/eventlogger/post');
    is_deeply(
        $req,
        {
            type => 'SessionEvent',
            sessionId => 1,
            owner => 'owner_value',
            time => 42,
            messages => [
                {
                    type => 't1'
                }
            ]
        }
    );
}

log_SendIntervalIsSetToZero_SendsMessagesImmediatelly:
{
    my $sendMessageCount = 0;
    $wsMock->mock(send => sub { ++$sendMessageCount; });

    _init(
        remoteSettings => {
            sendMessageIntervalSec => 0,
            filters => [{ messageType => 't1' }]
        }
    );

    logMsg({ type => 't1' });
    is($sendMessageCount, 1);
    logMsg({ type => 't1' });
    is($sendMessageCount, 2);
    logMsg({ type => 't1' });
    is($sendMessageCount, 3);
}

log_SendIntervalIsSetToPossitiveValue_AggregatesMessages:
{
    my $sendInvokesCount = 0;
    my @messages;
    $wsMock->mock(
        send => sub
        {
            my ($uri, $sessionEventMessage) = @_;
            ++$sendInvokesCount;
            @messages = @{ $sessionEventMessage->{messages} };
            return 0;
        }
    );
    _init(
        remoteSettings => {
            sendMessageIntervalSec => 100,
            filters => [{ messageType => 't1' }, { messageType => 't2' }, { messageType => 't3' }]
        }
    );

    logMsg({ type => 't1' });
    logMsg({ type => 't2' });
    logMsg({ type => 't3' });

    is($sendInvokesCount, 0);

    RemoteEventLogger::shutdown();

    is($sendInvokesCount, 1);
    is_deeply(
        \@messages,
        [
            {
                type => 't1'
            },
            {
                type => 't2'
            },
            {
                type => 't3'
            }
        ]
    );
}

log_NoFilters_DoesNotSendAnyMessages:
{
    my $timeUtilsMock = new Test::MockModule('TimeUtils');
    my $sendMessageCount = 0;
    $wsMock->mock(
        send => sub
        {
            ++$sendMessageCount;
        }
    );

    _init();

    logMsg({ type => 't1' });
    logMsg({ type => 't2' });
    logMsg({ type => 't3' });
    RemoteEventLogger::shutdown();
    is($sendMessageCount, 0);
}

log_SendFailLimitExceeded_StopsSendingMessagesAndStopLogWarnings:
{
    my $loggerMock = new Test::MockModule('Logger');
    my $sendInvokesCount = 0;
    $wsMock->mock(
        send => sub
        {
            ++$sendInvokesCount;
            return 1;    #fail
        }
    );

    _init(
        remoteSettings => {
            filters => [{ messageType => 't1' }],
            sendRetries => 5
        }
    );

    my $warningLogsInvokesCount = 0;
    $loggerMock->mock(
        warning => sub
        {
            ++$warningLogsInvokesCount;
        }
    );

    for (my $i = 0; $i < 10; $i++)
    {
        logMsg({ type => 't1' });
    }

    is($sendInvokesCount, 5);
    is($warningLogsInvokesCount, 5);
}

log_KeepsMessagesInQueueAfterFailedSendAttemps:
{
    my @messages;
    my $invokesCount = 0;
    $wsMock->mock(
        send => sub
        {
            my (undef, $sessionEventMessage) = @_;
            ++$invokesCount;
            if ($invokesCount <= 3)
            {    #fail 3 times
                return 1;
            }
            @messages = @{ $sessionEventMessage->{messages} };
            return 0;
        }
    );

    _init(
        remoteSettings => {
            filters => [{ messageType => 't1' }, { messageType => 't2' }, { messageType => 't3' }, { messageType => 't4' },],
            sendRetries => 5
        }
    );

    logMsg({ type => 't1' });
    logMsg({ type => 't2' });
    logMsg({ type => 't3' });
    logMsg({ type => 't4' });
    is_deeply(
        \@messages,
        [
            {
                type => 't1'
            },
            {
                type => 't2'
            },
            {
                type => 't3'
            },
            {
                type => 't4'
            }
        ]
    );
}

log_DoesNotSendAnyMessagesAfterShutdown:
{
    my $sendRetries = 10;
    _init(
        remoteSettings => {
            sendMessageIntervalSec => 0.001,
            filters => [{ messageType => 't1' },],
            sendRetries => $sendRetries
        }
    );
    RemoteEventLogger::shutdown();

    my $sendInvokesCount = 0;
    $wsMock->mock(
        send => sub
        {
            ++$sendInvokesCount;
            return 0;
        }
    );

    logMsg({ type => 't1' });
    is($sendInvokesCount, 0);
}

done_testing();

sub logMsg
{
    my ($msg) = @_;
    RemoteEventLogger::log($msg);
}

sub _init
{
    my %params = @_;

    my $rs = $params{remoteSettings};
    my $remoteSettings = {
        sessionId => $rs->{sessionId} ? $rs->{sessionId} : 123,
        sendMessageIntervalSec => $rs->{sendMessageIntervalSec} ? $rs->{sendMessageIntervalSec} : 0,
        filters => $rs->{filters} ? $rs->{filters} : [],
        sendRetries => $rs->{sendRetries}
    };

    my $lc = $params{localSettings};
    if (not defined($lc))
    {
        $lc = {};
    }
    my $remoteLoggerMock = new Test::MockModule('RemoteEventLogger');
    $remoteLoggerMock->mock(_getRemoteSettings => sub { return $remoteSettings; });

    RemoteEventLogger::initialize(
        owner => $lc->{owner} ? $lc->{owner} : 'owner_value',
        uri => $lc->{url} ? $lc->{url} : 'uri_value',
    );
}
