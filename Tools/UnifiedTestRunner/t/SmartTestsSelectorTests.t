use warnings;
use strict;

use Test::More;

use Dirs;
use lib Dirs::external_root();

use Test::MockModule;
use Test::Trap;
use TestHelpers::TestPlan;
use JSON;
use Capabilities;

BEGIN { use_ok('SmartTestsSelector'); }

can_ok('SmartTestsSelector', 'queryTests');

queryTests_NoParameters_ReturnsEmptyResult:
{
    my $result = SmartTestsSelector::queryTests();
    is($result, undef);
}
my $capabilities = Capabilities::getCapabilities();
my %defaultQueryTestsArgs = (
    uri => 'hoarder_uri',
    timeout => 30,
    branch => 'trunk',
    submitter => 'http://katana.com',
    configId => 'configId',
    testPlan => TestHelpers::TestPlan::createTestPlanFromTestObjects('Test1', 'Test2'),
    revision => '1234',
    build => '5',
    capabilities => $capabilities,
);

queryTests_MakesProperHttpRequest:
{
    my %result = _queryTests(%defaultQueryTestsArgs, httpResponseCode => 200,);

    my $capturedRequest = $result{request};
    is($capturedRequest->uri(), $defaultQueryTestsArgs{uri});
    is($capturedRequest->header('Content_Type'), 'application/json');
    is($capturedRequest->header('Accept'), 'application/json');
    my $requestContent = jsonToObj($capturedRequest->content());
    my $capabilities = Capabilities::getCapabilities();
    use Test::More;
    use Data::Dumper;
    is_deeply(
        $requestContent,
        {
            Branch => 'trunk',
            Submitter => 'http://katana.com',
            ConfigId => 'configId',
            Revision => '1234',
            Tests => [{ Name => 'Test1' }, { Name => 'Test2' }],
            BuildNumber => '5',
            Capabilities => $capabilities
        }
    );
}

queryTests_BadResponse_ReturnsEmptyResult:
{
    my %result = _queryTests(
        %defaultQueryTestsArgs,
        httpResponseCode => HTTP::Status::HTTP_INTERNAL_SERVER_ERROR,
        responseContent => '[{"Name": "Test1","Resolution": 0},{"Name": "Test2","Resolution": 0}]'
    );

    is($result{response}, undef);
    is($result{skippedTests}, undef);
}

queryTests_CheckTimeoutParamWhenPassed:
{
    my %result = _queryTests(%defaultQueryTestsArgs, httpResponseCode => 200);

    is($result{timeout}, $defaultQueryTestsArgs{timeout});
}

queryTests_CheckTimeoutParamWhenNotPassed:
{
    my %args = %defaultQueryTestsArgs;
    $args{timeout} = undef;

    my %result = _queryTests(%args, httpResponseCode => 200);

    is($result{timeout}, 15);
}

queryTests_RunAllTests_ReturnsCorrespondingTestPlanAndEmptySkippedList:
{
    my %result = _queryTests(
        %defaultQueryTestsArgs,
        httpResponseCode => 200,
        responseContent => '[{"Name": "Test1","Resolution": 0},{"Name": "Test2","Resolution": 0}]'
    );

    my @testItemsToRun = $result{testPlan}->getAllTestItems();

    my $testItem1 = new TestPlanItem(name => 'Test1', testCaseCount => 1);
    my $testItem2 = new TestPlanItem(name => 'Test2', testCaseCount => 1);

    is_deeply(\@testItemsToRun, [$testItem1, $testItem2]);
    is_deeply($result{skippedTests}, []);
}

queryTests_NewProtocolResponseContent_ReturnsCorrespondingResultAndSessionId:
{
    my %result = _queryTests(
        %defaultQueryTestsArgs,
        httpResponseCode => 200,
        responseContent => '{"SmartSelectSessionId":13, "Resolutions":[{"Name": "Test1","Resolution": 1},{"Name": "Test2","Resolution": 1}]}'
    );

    my @testItemsToRun = $result{testPlan}->getAllTestItems();
    is_deeply(\@testItemsToRun, []);
    is_deeply($result{skippedTests}, ['Test1', 'Test2']);
    is($result{id}, 13);
}

queryTests_OldProtocolResponseContent_ReturnsCorrespondingResultAndUndefSessionId:
{
    my %result = _queryTests(
        %defaultQueryTestsArgs,
        httpResponseCode => 200,
        responseContent => '[{"Name": "Test1","Resolution": 1},{"Name": "Test2","Resolution": 1}]'
    );

    my @testItemsToRun = $result{testPlan}->getAllTestItems();
    is_deeply(\@testItemsToRun, []);
    is_deeply($result{skippedTests}, ['Test1', 'Test2']);
    is($result{id}, undef);
}

queryTests_SkipAllTests_ReturnsEmptyTestPlanAndCorrespondingSkippedList:
{
    my %result = _queryTests(
        %defaultQueryTestsArgs,
        httpResponseCode => 200,
        responseContent => '[{"Name": "Test1","Resolution": 1},{"Name": "Test2","Resolution": 1}]'
    );

    my @testItemsToRun = $result{testPlan}->getAllTestItems();
    is_deeply(\@testItemsToRun, []);
    is_deeply($result{skippedTests}, ['Test1', 'Test2']);
}

queryTests_MixedResolutionsTests_ReturnsCorrespondingTestPlanAndSkippedList:
{
    my %result = _queryTests(
        %defaultQueryTestsArgs,
        httpResponseCode => 200,
        responseContent => '[{"Name": "Test1","Resolution": 0},{"Name": "Test2","Resolution": 1}]'
    );

    my @testItemsToRun = $result{testPlan}->getAllTestItems();
    my $testItem1 = new TestPlanItem(name => 'Test1', testCaseCount => 1);

    is_deeply(\@testItemsToRun, [$testItem1]);
    is_deeply($result{skippedTests}, ['Test2']);
}

queryTests_WithoutAllParams_Croaks:
{
    _testMissingParam('revision', %defaultQueryTestsArgs);
    _testMissingParam('branch', %defaultQueryTestsArgs);
    _testMissingParam('submitter', %defaultQueryTestsArgs);
    _testMissingParam('configId', %defaultQueryTestsArgs);
    _testMissingParam('testPlan', %defaultQueryTestsArgs);
    _testMissingParam('uri', %defaultQueryTestsArgs);
}

done_testing();

sub _testMissingParam
{
    my ($missingParamName, %args) = @_;
    $args{$missingParamName} = undef;
    my @trap = trap
    {
        _queryTests(%args);
    };
    like($trap->die, qr/$missingParamName/, 'croak when ' . $missingParamName . ' is not specified');
}

sub _queryTests
{
    my %args = @_;
    my $requestMock = new Test::MockModule('Request');
    my %sendArgs;
    my $request;
    my $timeout;
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
    my ($testPlan, $skippedTests, $id) = SmartTestsSelector::queryTests(%args);
    my %result;
    $result{request} = $sendArgs{request};
    $result{testPlan} = $testPlan;
    $result{skippedTests} = $skippedTests;
    $result{timeout} = $sendArgs{timeout};
    $result{id} = $id;
    return %result;
}
