use warnings;
use strict;

use Test::More;
use Dirs;
use lib Dirs::external_root();
use Test::MockObject;
use TestResult;
use SuiteResult;
use TestHelpers::ArrayAssertions qw (arraysAreEqual);
use Hoarder::Presentation::Test;
use ArtifactsMap;
use ArtifactsMapEntry;

BEGIN { use_ok('Hoarder::TestSessionSubmissionRequestBuilder'); }

CREATION:
{
    can_ok('Hoarder::TestSessionSubmissionRequestBuilder', 'new');
    my $requestBuilder = Hoarder::TestSessionSubmissionRequestBuilder->new();
    isa_ok($requestBuilder, 'Hoarder::TestSessionSubmissionRequestBuilder');
}

BUILD:
{
    my $requestBuilder = Hoarder::TestSessionSubmissionRequestBuilder->new();
    can_ok($requestBuilder, 'build');
    my $collector1 = new Test::MockObject();
    $collector1->mock(name => sub { return 'collector1' });
    $collector1->mock(data => sub { return {}; });

    my $collector2 = new Test::MockObject();
    $collector2->mock(name => sub { return 'collector2' });
    $collector2->mock(data => sub { return {}; });

    my @collectors = ($collector1, $collector2);
    my $request = $requestBuilder->build(
        collectors => \@collectors,
        suite_results => [],
        smartSelectSessionId => 42
    );
    my %data = %{ $request->data() };
    is(keys(%data), 5);

    ok(exists($data{'collector1'}));
    ok(exists($data{'collector2'}));
    ok(exists($data{'suites'}));
    is($data{'version'}, 1);
    is(scalar(@{ $data{'suites'} }), 0);
    is($data{'smartSelectSessionId'}, 42);

TEST_RESULTS_PROCESSING:
    {
        my $tr1 = new TestResult();
        $tr1->setTest('Test1');
        my $suiteResult = new SuiteResult(name => 'native', platform => 'standalone');
        my @testResults = ($tr1);
        $suiteResult->addTestResults(@testResults);

        my @suiteResults = ($suiteResult);

        $request = $requestBuilder->build(collectors => \@collectors, suite_results => \@suiteResults);
        %data = %{ $request->data() };
        ok(exists($data{'suites'}));

        my @suites = @{ $data{'suites'} };
        is(scalar(@suites), 1);

        my %suiteData = %{ $suites[0]->data() };
        is($suiteData{name}, 'native');

    TWO_TESTS_IN_SAME_SUITE:
        {
            my $tr2 = new TestResult();
            $tr2->setTest('Test2');
            $suiteResult->addTestResult($tr2);
            $request = $requestBuilder->build(collectors => \@collectors, suite_results => \@suiteResults);
            %data = %{ $request->data() };
            @suites = @{ $data{suites} };
            is(scalar(@suites), 1);
        }

    TWO_TESTS_IN_DIFFERENT_SUITES:
        {
            my $tr2 = new TestResult();
            $tr2->setTest('Test3');
            my $suiteResult = new SuiteResult(name => 'runtime', platform => 'editor');
            $suiteResult->addTestResult($tr2);
            push(@suiteResults, $suiteResult);
            $request = $requestBuilder->build(collectors => \@collectors, suite_results => \@suiteResults);
            %data = %{ $request->data() };
            @suites = @{ $data{suites} };
            is(scalar(@suites), 2);
            %suiteData = %{ $suites[0]->data() };
            is($suiteData{name}, 'native');
            is($suiteData{platform}, 'standalone');
            %suiteData = %{ $suites[1]->data() };
            is($suiteData{name}, 'runtime');
            is($suiteData{platform}, 'editor');
        }
    }
}

TEST_RESULT_IS_STORED_CORRECTLY:
{
    my $requestBuilder = Hoarder::TestSessionSubmissionRequestBuilder->new();
    my $tr = new TestResult();
    $tr->setTest('Test1');
    $tr->setFixture('ShaderTests');
    $tr->setTime('321');
    $tr->setState(TestResult::FAILURE);
    $tr->setMessage('Hej. Hvordan har du det?');
    my $suiteResult = new SuiteResult(name => 'integration', platform => 'TestABV');
    $suiteResult->addTestResult($tr);
    my @suiteResults = ($suiteResult);

    my @testresults = ($tr);
    my (@collectors);
    my $request = $requestBuilder->build(collectors => \@collectors, suite_results => \@suiteResults);
    my %data = %{ $request->data() };
    my @suites = @{ $data{suites} };
    my %suiteData = %{ $suites[0]->data() };
    my @tests = @{ $suiteData{tests} };
    my $test = $tests[0];
    my %testData = %{ $test->data() };

    is($suiteData{name}, 'integration');
    is($suiteData{platform}, 'TestABV');
    is($testData{name}, 'Test1');
    is($testData{fixture}, 'ShaderTests');
    is($testData{time}, '321');
    is($testData{state}, TestResult::FAILURE);
    is($testData{message}, 'Hej. Hvordan har du det?');
}

CAN_STORE_RESULT_FROM_COLLECTOR_WHICH_RETURNS_ARRAY:
{
    my $requestBuilder = Hoarder::TestSessionSubmissionRequestBuilder->new();
    my $collector1 = new Test::MockObject();
    $collector1->mock(name => sub { return 'collector1' });
    $collector1->mock(data => sub { my @arr = ('a', 'b', 'c'); return \@arr; });

    my @collectors = ($collector1);
    my $request = $requestBuilder->build(collectors => \@collectors, suite_results => []);
    my %data = %{ $request->data() };

    my @values = @{ $data{collector1} };
    is(scalar(@values), 3);
}

IF_COLLECTOR_RETURNS_UNDEF_DATA_IS_NOT_STORED_IN_REQUEST:
{
    my $requestBuilder = Hoarder::TestSessionSubmissionRequestBuilder->new();
    my $collector1 = new Test::MockObject();
    $collector1->mock(name => sub { return 'collector1' });
    $collector1->mock(data => sub { return undef; });

    my @collectors = ($collector1);
    my $request = $requestBuilder->build(collectors => \@collectors, suite_results => []);
    my $data = $request->data();
    ok(not exists($data->{collector1}));
}

FORMS_CORREST_REQUEST_FOR2SUITERESULTS:
{
    my $requestBuilder = Hoarder::TestSessionSubmissionRequestBuilder->new();
    my $tr1 = new TestResult();
    $tr1->setTest('Test1');

    my $tr2 = new TestResult();
    $tr2->setTest('Test2');

    my $artifactsMap = new ArtifactsMap();
    my $artifactEnty = new ArtifactsMapEntry('Test1');

    my $suiteResult1 = new SuiteResult(name => 'editor', platform => 'editor');
    $suiteResult1->addTestResult($tr1);
    $suiteResult1->setTestResultXmlFilePath('xmlpath1');

    my $suiteResult2 = new SuiteResult(name => 'editor', platform => 'editor');
    $suiteResult2->addTestResult($tr2);
    $suiteResult2->setTestResultXmlFilePath('xmlpath2');

    my @suiteResults = ($suiteResult1, $suiteResult2);

    my (@collectors);
    my $request = $requestBuilder->build(collectors => \@collectors, suite_results => \@suiteResults);

    my %data = %{ $request->data() };
    my @suites = @{ $data{suites} };

    is(scalar(@suites), 2);
    is($suites[0]->{testresultsxml}, 'xmlpath1');
    is($suites[1]->{testresultsxml}, 'xmlpath2');
}

done_testing();
