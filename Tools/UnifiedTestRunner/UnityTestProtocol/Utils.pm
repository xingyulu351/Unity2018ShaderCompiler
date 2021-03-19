package UnityTestProtocol::Utils;

use warnings;
use strict;

use Dirs;
use lib Dirs::external_root();
use JSON;
use FileUtils;
use StringUtils;

require Exporter;
our @ISA = qw(Exporter);
our %EXPORT_TAGS = (
    all => [
        qw(
            &getDirName
            &getMessage
            &getFirst
            &getMessages
            &isBeginMessage
            &isEndMessage
            &isAction
            &isActionBegin
            &isActionEnd
            &isTestStatus
            &isTestBegin
            &isTestEnd
            &isTestPlan
            &hasErrors
            )
    ],
);

our @EXPORT_OK = (@{ $EXPORT_TAGS{'all'} });

sub getFirst
{
    my (%args) = @_;
    my $predicate = $args{predicate};
    my $result;
    FileUtils::scanLines(
        file => $args{fileName},
        func => sub
        {
            my ($line) = @_;
            my $msg = getMessage($line);
            if (not defined($msg))
            {
                return 1;
            }
            my $res = &{$predicate}($msg);
            if ($res)
            {
                $result = $msg;
                return 0;    # stop scan
            }
            return 1;
        }
    );

    return $result;
}

sub getMessages
{
    my (%args) = @_;
    my $predicate = $args{predicate};
    my @result;
    FileUtils::scanLines(
        file => $args{fileName},
        func => sub
        {
            my ($line) = @_;
            my $msg = getMessage($line);
            if (not defined($msg))
            {
                return 1;
            }
            my $res = &{$predicate}($msg);
            if ($res)
            {
                push(@result, $msg);
            }
            return 1;
        }
    );
    return @result;
}

sub getMessage
{
    my ($line) = @_;
    if (StringUtils::isNullOrEmpty($line))
    {
        return undef;
    }
    my ($protocolMessage) = ($line =~ m /##utp:(.*)/);
    if (not defined($protocolMessage))
    {
        return undef;
    }
    return jsonToObj($protocolMessage);
}

sub isBeginMessage
{
    my ($msg) = @_;
    return _result($msg->{phase} eq "Begin");
}

sub isEndMessage
{
    my ($msg) = @_;
    return _result($msg->{phase} eq "End");
}

sub isAction
{
    my ($msg) = @_;
    return _result($msg->{type} eq "Action");
}

sub isActionBegin
{
    my ($msg) = @_;
    return _result(isAction($msg) && isBeginMessage($msg));
}

sub isActionEnd
{
    my ($msg) = @_;
    return _result(isAction($msg) && !isBeginMessage($msg));
}

sub isTestStatus
{
    my ($msg) = @_;
    return _result($msg->{type} eq "TestStatus");
}

sub isTestBegin
{
    my ($msg) = @_;
    return _result(isTestStatus($msg) && isBeginMessage($msg));
}

sub isTestEnd
{
    my ($msg) = @_;
    return _result(isTestStatus($msg) && !isBeginMessage($msg));
}

sub isTestPlan
{
    my ($msg) = @_;
    return _result($msg->{type} eq "TestPlan");
}

sub hasErrors
{
    my ($msg) = @_;
    return _result($msg->{errors} and scalar(@{ $msg->{errors} }) > 0);
}

sub _result
{
    my ($res) = @_;
    if ($res)
    {
        return 1;
    }

    return 0;
}

1;
