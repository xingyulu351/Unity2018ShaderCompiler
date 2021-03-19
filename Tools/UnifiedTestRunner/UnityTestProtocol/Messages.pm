package UnityTestProtocol::Messages;

use warnings;
use strict;

use TimeUtils;
use Hoarder::Collectors;

sub makeSessionEndMessage
{
    my ($summary) = @_;
    my $result = {};
    $result->{type} = 'SessionEnd';
    $result->{time} = TimeUtils::getUTCNowMs();
    $result->{summary} = $summary;
    return $result;
}

sub makeSmartSelectMessage
{
    my ($smartSelectData) = @_;
    my $result = {};
    $result->{type} = 'SmartSelect';
    $result->{time} = TimeUtils::getUTCNowMs();
    $result->{total} = $smartSelectData->{total};
    $result->{skipped} = $smartSelectData->{skipped};
    return $result;
}

sub makeSessionStartMessage
{
    my $result = {};
    $result->{type} = 'SessionStart';
    $result->{time} = TimeUtils::getUTCNowMs();
    _addCollectorsData($result);
    return $result;
}

sub makeEnvironmentVariablesMessage
{
    my ($env) = @_;
    my $result = {};
    $result->{type} = 'EnvVars';
    $result->{time} = TimeUtils::getUTCNowMs();
    $result->{vars} = $env;
    return $result;
}

sub _addCollectorsData
{
    my ($result) = @_;
    $result->{data} = {};
    my @collectors = Hoarder::Collectors::getAllCollectors();
    foreach my $c (@collectors)
    {
        my $data = $c->data();
        if (defined($data))
        {
            $result->{data}{ $c->name() } = $data;
        }
    }
}

1;
