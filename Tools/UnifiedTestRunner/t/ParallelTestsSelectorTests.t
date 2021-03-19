use warnings;
use strict;

use Test::More;
use TestHelpers::TestPlan;
use Dirs;
use lib Dirs::external_root();
use Test::MockModule;
use JSON;
use Test::Trap;

BEGIN { use_ok('ParallelTestsSelector'); }

queryTests_NoParameters_ReturnsEmptyResult:
{
    my %result = _queryPartitions();
    is($result{paritions}, undef);
}

my %defaultArgs = (
    uri => 'hoarder_uri',
    partitionCount => 4,
    timeout => 30,
    branch => 'trunk',
    submitter => 'http://katana.com',
    configId => 'configId',
    testPlan => TestHelpers::TestPlan::createTestPlanFromTestObjects('Test1', 'Test2'),
    revision => '1234',
    filerUsingSmartSelect => 1,
    timeout => 30,
    retries => 2,
);

my $responseContent = qq (
{
  "FilteredTests": [],
  "Partitions": [
    [
      {
        "Name": "test1"
      }
    ],
    [
      {
        "Name": "test2"
      }
    ]
  ]
});

queryParitions_MakesProperHttpRequest:
{
    my %result = _queryPartitions(
        %defaultArgs,
        responseContent => $responseContent,
        httpResponseCode => 200,
    );

    my $capturedRequest = $result{request};
    is($capturedRequest->uri(), $defaultArgs{uri});
    is($capturedRequest->method(), 'POST');
    is($capturedRequest->header('Content_Type'), 'application/json');
    is($capturedRequest->header('Accept'), 'application/json');

    my $requestContent = jsonToObj($capturedRequest->content());

    my $sendArgs = $result{sendArgs};
    is($sendArgs->{timeout}, $defaultArgs{timeout});
    is($sendArgs->{retries}, $defaultArgs{retries});
    is_deeply(
        $requestContent,
        {
            Branch => 'trunk',
            PartitionCount => 4,
            Submitter => 'http://katana.com',
            ConfigId => 'configId',
            Revision => '1234',
            Tests => [{ Name => 'Test1' }, { Name => 'Test2' }],
            FilterUsingSmartSelect => 0
        }
    );

    is_deeply($result{response}, $responseContent);
}

queryParitions_DoesNotIncludePartitionsCountIfNotSpecified:
{
    my %args = %defaultArgs;
    delete $args{partitionCount};
    my %result = _queryPartitions(
        %args,
        responseContent => $responseContent,
        httpResponseCode => 200,
    );

    my $capturedRequest = $result{request};
    my $requestContent = jsonToObj($capturedRequest->content());
    is_deeply(
        $requestContent,
        {
            Branch => 'trunk',
            Submitter => 'http://katana.com',
            ConfigId => 'configId',
            Revision => '1234',
            Tests => [{ Name => 'Test1' }, { Name => 'Test2' }],
            FilterUsingSmartSelect => 0
        }
    );

    is_deeply($result{response}, $responseContent);
}

queryTests_BadResponse_ReturnsEmptyResult:
{
    my %result = _queryPartitions(
        %defaultArgs,
        httpResponseCode => HTTP::Status::HTTP_INTERNAL_SERVER_ERROR,
        responseContent => $responseContent,
    );

    is($result{response}, undef);
}

queryPartitionsById_MakesProperHttpRequest:
{
    my %result = _queryPartitionsById(
        id => 42,
        uri => $defaultArgs{uri},
        timeout => $defaultArgs{timeout},
        retries => $defaultArgs{retries},
        responseContent => $responseContent,
        httpResponseCode => 200,
    );
    my $capturedRequest = $result{request};
    is($capturedRequest->uri(), "$defaultArgs{uri}/42");
    is($capturedRequest->method(), 'GET');
    is($capturedRequest->header('Accept'), 'application/json');

    my $sendArgs = $result{sendArgs};
    is($sendArgs->{timeout}, $defaultArgs{timeout});
    is($sendArgs->{retries}, $defaultArgs{retries});
    is_deeply($result{response}, $responseContent);
}

missingParamsQueryPartitions:
{
    my $method = \&_queryPartitions;
    _testMissingParam('revision', $method, %defaultArgs);
    _testMissingParam('branch', $method, %defaultArgs);
    _testMissingParam('submitter', $method, %defaultArgs);
    _testMissingParam('configId', $method, %defaultArgs);
    _testMissingParam('testPlan', $method, %defaultArgs);
    _testMissingParam('uri', $method, %defaultArgs);
    _testMissingParam('timeout', $method, %defaultArgs);
}

missingParamsQueryPartitionsById:
{
    my $method = \&_queryPartitionsById;
    my %args = (
        uri => $defaultArgs{uri},
        id => 42,
        timeout => $defaultArgs{timeout},
        retries => $defaultArgs{retries}
    );
    _testMissingParam('uri', $method, %args);
    _testMissingParam('id', $method, %args);
    _testMissingParam('timeout', $method, %args);
    _testMissingParam('retries', $method, %args);
}

sub _testMissingParam
{
    my ($missingParamName, $method, %args) = @_;
    $args{$missingParamName} = undef;
    my @trap = trap
    {
        &$method(%args);
    };
    like($trap->die, qr/$missingParamName/, 'croak when ' . $missingParamName . ' is not specified');
}

done_testing();

sub _queryPartitions
{
    my %args = @_;
    my $requestMock = new Test::MockModule('Request');
    my %sendArgs;
    my $request;
    $requestMock->mock(
        send => sub
        {
            %sendArgs = @_;
            my $response = new HTTP::Response();
            $response->code($args{httpResponseCode});
            $response->content($args{responseContent});
            return $response;
        }
    );
    my $paritions = ParallelTestsSelector::queryPartitions(%args);
    my %result;
    $result{response} = $paritions;
    $result{request} = $sendArgs{request};
    $result{sendArgs} = \%sendArgs;
    return %result;
}

sub _queryPartitionsById
{
    my %args = @_;
    my $requestMock = new Test::MockModule('Request');
    my %sendArgs;
    my $request;
    $requestMock->mock(
        send => sub
        {
            %sendArgs = @_;
            my $response = new HTTP::Response();
            $response->code($args{httpResponseCode});
            $response->content($args{responseContent});
            return $response;
        }
    );
    my $paritions = ParallelTestsSelector::queryPartitionsById(%args);
    my %result;
    $result{response} = $paritions;
    $result{request} = $sendArgs{request};
    $result{sendArgs} = \%sendArgs;
    return %result;
}
