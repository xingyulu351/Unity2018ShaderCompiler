package ParallelTestsSelector;

use warnings;
use strict;

use Dirs;
use lib Dirs::external_root();
use JSON;
use LWP::UserAgent;
use Logger;
use Request;
use ParamUtils qw (verifyParamExistsOrCroak);

sub queryPartitions
{
    my %args = @_;
    if (not %args)
    {
        return undef;
    }

    verifyParamExistsOrCroak('uri', %args);
    verifyParamExistsOrCroak('revision', %args);
    verifyParamExistsOrCroak('branch', %args);
    verifyParamExistsOrCroak('submitter', %args);
    verifyParamExistsOrCroak('configId', %args);
    verifyParamExistsOrCroak('testPlan', %args);
    verifyParamExistsOrCroak('timeout', %args);
    verifyParamExistsOrCroak('retries', %args);

    return _query(_makeRequest(%args), $args{timeout}, $args{retries});
}

sub queryPartitionsById
{
    my %args = @_;

    verifyParamExistsOrCroak('uri', %args);
    verifyParamExistsOrCroak('id', %args);
    verifyParamExistsOrCroak('timeout', %args);
    verifyParamExistsOrCroak('retries', %args);
    my $headers = HTTP::Headers->new(Accept => 'application/json',);

    my $request = HTTP::Request->new(
        GET => "$args{uri}/$args{id}",
        $headers
    );

    return _query($request, $args{timeout}, $args{retries});
}

sub _query
{
    my ($request, $timeout, $retries) = @_;
    my $response = Request::send(
        request => $request,
        timeout => $timeout,
        retries => $retries
    );
    if ($response->is_error())
    {
        return undef;
    }
    return $response->content();
}

sub _makeRequest
{
    my %args = @_;
    my $headers = HTTP::Headers->new(
        Content_type => 'application/json',
        Accept => 'application/json',
    );

    return HTTP::Request->new(
        POST => $args{uri},
        $headers,
        _getJsonContentFromRequest(%args),
    );
}

sub _getJsonContentFromRequest
{
    my %args = @_;

    my $requestDataObj = {
        Branch => $args{branch},
        Submitter => $args{submitter},
        ConfigId => $args{configId},
        Tests => _createTestItems($args{testPlan}),
        Revision => $args{revision},
        FilterUsingSmartSelect => $args{filterUsingSmartSelect} || 0
    };
    if ($args{partitionCount})
    {
        $requestDataObj->{PartitionCount} = $args{partitionCount};
    }
    return objToJson($requestDataObj);
}

sub _createTestItems
{
    my ($testPlan) = @_;
    my @tests;
    my @testItems = $testPlan->getAllTestItems();
    foreach my $t (@testItems)
    {
        push(@tests, { Name => $t->getName() });
    }
    return \@tests;
}

1;
