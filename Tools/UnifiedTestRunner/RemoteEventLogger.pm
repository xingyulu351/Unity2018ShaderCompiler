package RemoteEventLogger;

use warnings;
use strict;

use RemoteEventLoggerWebService;
use TimeUtils;

use threads;
use Thread::Queue;

use Dirs;
use lib Dirs::external_root();

use Time::HiRes;
use AnyEvent;
use UnityTestProtocol::MessageFilter;
use Data::Dumper;

use threads;
use threads::shared;
use Thread::Queue;
use Thread::Semaphore;

my $sessionId : shared;
my $uri : shared;
my $owner : shared;
my $sendMessageIntervalSec : shared;
my $sendRetries : shared;
my $sendRetryNo : shared;
my $filter;

my $messageQueue;
my $thr;

sub initialize
{
    my %params = @_;
    $owner = $params{owner};
    if (not $owner)
    {
        return;
    }

    $messageQueue = new Thread::Queue();
    $uri = $params{uri};

    my $remoteSettings = _getRemoteSettings();

    $sendMessageIntervalSec = $remoteSettings->{sendMessageIntervalSec};
    $filter = new UnityTestProtocol::MessageFilter($remoteSettings->{filters});
    $sendRetries = $remoteSettings->{sendRetries};
    if (not defined($sendRetries))
    {
        $sendRetries = 5;
    }

    $sendRetryNo = 0;
    $sessionId = $remoteSettings->{sessionId};

    if (not $sendMessageIntervalSec)
    {
        return;
    }

    $thr = _startMessageAggregationThread($remoteSettings);
}

sub log
{
    my ($msg) = @_;
    my $notInitialized = not $messageQueue;
    if ($notInitialized)
    {
        return;
    }

    if ($filter and not $filter->check($msg))
    {
        return;
    }

    $messageQueue->enqueue($msg);

    my $sendImediately = not $sendMessageIntervalSec;
    if ($sendImediately)
    {
        _sendMessages();
    }
}

sub _sendMessages
{
    if (not $uri)
    {
        return;
    }
    my $pendingElementsCount = $messageQueue->pending();
    if ($pendingElementsCount == 0)
    {
        return;
    }
    if ($sendRetryNo >= $sendRetries)
    {
        return;
    }

    my @messages = $messageQueue->extract(0, $pendingElementsCount);
    my $req = shared_clone(_wrap(@messages));
    my $sendRes = RemoteEventLoggerWebService::send("$uri/eventlogger/post", $req);

    if ($sendRes != 0)
    {
        ++$sendRetryNo;
        if ($sendRetryNo > $sendRetries)
        {
            Logger::warning("Give up on trying to send data to $uri");
        }
        else
        {
            Logger::warning("Failed to send messages to $uri. Attempt $sendRetryNo/$sendRetries");
        }

        # in case if other messages has been inserted in other thread,
        # put all the messages we've just tries to send in the beggining
        # to keep a relative order
        $messageQueue->insert(0, @messages);
    }
    else
    {
        $sendRetryNo = 0;
    }
}

sub shutdown
{
    if ($thr)
    {
        $thr->kill('KILL');
    }
    _sendMessages();
}

sub _startMessageAggregationThread
{
    my ($remoteSettings) = @_;
    my $intitSemaphore = new Thread::Semaphore();
    $intitSemaphore->down();
    my $thr = threads->create(\&_messageAggregationThreadFunc, $intitSemaphore, $remoteSettings);
    $intitSemaphore->down();    #wait until thread is initialized
    $thr->detach();
    return $thr;
}

sub _getRemoteSettings
{
    if (not $uri)
    {
        return {};
    }
    Logger::verbose("Fetching logging filters from $uri");
    my $req = UnityTestProtocol::Messages::makeSessionStartMessage();
    $req = shared_clone($req->{data});
    my ($sendRes, $remoteSettings) = RemoteEventLoggerWebService::send("$uri/sessions/init", $req);
    if ($sendRes != 0)
    {
        Logger::warning("Failed to fetch remote logging setting due to the error. Remote event logging is disabled");
        $remoteSettings = {};
    }

    Logger::verbose("Remote settings: \n" . Dumper($remoteSettings));
    return $remoteSettings;
}

sub _wrap
{
    my (@messages) = @_;
    my $req = {
        type => 'SessionEvent',
        owner => $owner,
        time => TimeUtils::getUTCNowMs(),
        sessionId => $sessionId,
        messages => \@messages,
    };
    return $req;
}

sub _messageAggregationThreadFunc
{
    my ($intitSemaphore, $remoteSettings) = @_;
    my $cv = AnyEvent->condvar();
    $SIG{'KILL'} = sub
    {
        $cv->send();
    };

    my $timer = AnyEvent->timer(
        after => $remoteSettings->{sendMessageIntervalSec},
        interval => $remoteSettings->{sendMessageIntervalSec},
        cb => sub { _sendMessages() }
    );

    $intitSemaphore->up();    #release semaphore

    # AnyEvent  expects somebody else to run the event
    # loop and will only block when necessary
    $cv->recv();
    undef($timer);
}

1;
