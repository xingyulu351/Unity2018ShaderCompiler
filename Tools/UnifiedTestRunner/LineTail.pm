package LineTail;

use warnings;
use strict;

use IO::File;
use Dirs;
use lib Dirs::external_root();
use Readonly;
use Time::HiRes;
use threads;
use threads::shared;
use Carp qw (croak);
use English qw( -no_match_vars );

Readonly my $DEFAULT_RETRIES_COUNT => 5;
Readonly my $EMPTY_LINE => q ();
Readonly my $FILE_CHECK_INTERVAL => 0.25;

sub new
{
    my ($pkg, %params) = @_;
    my $object = {};
    bless($object, $pkg);
    $object->{file} = $params{'file'};
    $object->{receiver} = $params{'receiver'};
    $object->{arg} = $params{'arg'};
    return $object;
}

sub getFile
{
    my ($this) = @_;
    return $this->{file};
}

sub getReceiver
{
    my ($this) = @_;
    return $this->{receiver};
}

sub getArg
{
    my ($this) = @_;
    return $this->{arg};
}

my $retriesCount : shared = $DEFAULT_RETRIES_COUNT;

sub setRetriesCount
{
    my ($value) = @_;
    lock($retriesCount);
    $retriesCount = $value;
}

sub getRetriesCount
{
    my ($value) = @_;
    lock($retriesCount);
    my $result = $retriesCount;
    return $result;
}

sub waitForLog
{
    my ($filename) = @_;
    my $retries = getRetriesCount();

    while (1)
    {
        if ($retries-- == 0)
        {
            return;
        }
        my $openres = open(my $FH, '<', $filename);
        if (defined($openres) and $openres != 0)
        {
            return $FH;
        }
        sleep(1);
    }
    return;
}

READ_CYCLE:
{
    my $pendingLine = $EMPTY_LINE;

    sub readCycle
    {
        my ($FH, $receiver, $arg, $filename, $lineTail, $forceFlush) = @_;
        my $curpos = tell($FH);
        while (<$FH>)
        {
            my $line = $_;
            if ($line !~ m/\n/s)
            {
                $pendingLine .= $line;
            }
            else
            {
                $line = $pendingLine . $line;
                $pendingLine = $EMPTY_LINE;
                chomp($line);
                &{$receiver}($line, $arg, $filename);
            }
            $curpos = tell($FH);
        }
        if ($pendingLine && $forceFlush)
        {
            &{$receiver}($pendingLine, $arg, $filename);
            $pendingLine = $EMPTY_LINE;
            return;
        }
        seek($FH, $curpos, 0);    #reset eof again
    }
}

sub tail_file
{
    my ($filename, $receiver, $arg, $lineTail) = @_;
    my $FH = waitForLog($filename);
    if (not defined($FH))
    {
        return;
    }
    while (not $lineTail->getQuitFlag())
    {
        readCycle($FH, $receiver, $arg, $filename, $lineTail, 0);
        Time::HiRes::sleep($FILE_CHECK_INTERVAL);
    }
    readCycle($FH, $receiver, $arg, $filename, $lineTail, 1);
    my $closeRes = close($FH);
    if (not $closeRes)
    {
        croak("Can not close the file $filename. $OS_ERROR\n");
    }
    &{$receiver}(undef, $arg, $filename);
    return 0;
}

{

    sub start
    {
        my ($this) = shift();
        $this->{quitFlag} = 0;
        share($this->{quitFlag});
        $this->{thread} = threads->create(\&tail_file, $this->getFile(), $this->getReceiver(), $this->getArg(), $this);
    }

    sub stop
    {
        my $this = shift();
        $this->setQuitFlag(1);
        $this->{thread}->join();
    }
}

{
    my $quitFlag : shared = 0;

    sub setQuitFlag
    {
        my ($this, $value) = @_;
        lock($this->{quitFlag});
        $this->{quitFlag} = $value;
    }

    sub getQuitFlag
    {
        my ($this) = @_;
        lock($this->{quitFlag});
        my $result = $this->{quitFlag};
        return $result;
    }
}

1;
