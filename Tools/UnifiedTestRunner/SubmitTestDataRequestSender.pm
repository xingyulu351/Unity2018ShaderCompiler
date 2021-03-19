package SubmitTestDataRequestSender;

use warnings;
use strict;

use Dirs;
use lib Dirs::external_root();

use LWP::UserAgent;
use HTTP::Request::Common;
use HTTP::Response;

sub send
{
    my %args = @_;
    my $retriesCount = $args{retries};
    if (not $retriesCount)
    {
        $retriesCount = 1;
    }

    my $retryTimeout = $args{timeout};
    if (not defined($retryTimeout))
    {
        $retryTimeout = 0;
    }

    my $logger = $args{logger};
    if (not defined($logger))
    {
        $logger = sub { };
    }

    for (my $i = 0; $i < $retriesCount; ++$i)
    {
        my $result = _doSend($args{uri}, $args{header}, $args{files}, $logger);
        if ($result eq 1)
        {
            return 1;
        }

        my $lastIteration = ($i eq $retriesCount - 1);
        if (not $lastIteration)
        {
            _log($logger, "Send data failed. Sleep for $retryTimeout seconds before retrying\n");
            sleep($retryTimeout);
        }
    }
    return 0;
}

sub _doSend
{
    my ($uri, $reqestHeader, $referencedFiles, $logger) = @_;

    my $userAgent = new LWP::UserAgent();

    my (@refFilePairs);
    foreach my $ref (@$referencedFiles)
    {
        push(@refFilePairs, $ref, [$ref]);
    }

    my $response;
    if (scalar(@refFilePairs) eq 0)
    {
        $response = $userAgent->post(
            $uri,
            Content_Type => 'application/json',
            Content => $reqestHeader,
        );
    }
    else
    {
        $response = $userAgent->post(
            $uri,
            Content_Type => 'form-data',
            Content => [
                request => $reqestHeader,
                @refFilePairs
            ]
        );
    }

    _log($logger, "\n---Response\n" . $response->as_string() . "\n---\n");

    if ($response->is_success())
    {
        return 1;
    }

    _log($logger, "\n---Failure response:\n " . $response->content() . "\n---\n");

    return 0;
}

sub _log
{
    my ($logger, $message) = @_;
    &$logger($message);
}

1;
