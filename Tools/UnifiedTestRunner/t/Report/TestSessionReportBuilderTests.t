use warnings;
use strict;

use Test::More;

use TestResult;
use SuiteResult;
use Commands;
use TestHelpers::ArrayAssertions qw (isEmpty arraysAreEquivalent arraysAreEqual);
use Test::MockObject;

BEGIN { use_ok('Report::TestSessionReportBuilder') }

can_ok('Report::TestSessionReportBuilder', 'buildData');

BuildData_EmptySuiteResult_SerializedCorrectly:
{
    my $suiteResult = new SuiteResult(name => 'integration', platform => 'TestABV');
    $suiteResult->setDescription('description');
    my @suiteResults = ($suiteResult);
    my %reportData = Report::TestSessionReportBuilder::buildData(suiteResults => \@suiteResults);

    is($reportData{utrPrefix}, Commands::getUtrPrefix());
    my @suites = @{ $reportData{suites} };
    is(scalar(@suites), 1);
    my %suite = %{ $suites[0] };
    is($suite{name}, 'integration');
    is($suite{platform}, 'TestABV');
    is($suite{description}, 'description');
    isEmpty($suite{artifacts});
}

BuildData_SuiteResultWithArtifacts_SerializedCorrectly:
{
    my $suiteResult = new SuiteResult(name => 'integration', platform => 'TestABV', artifacts => ['/a/b/a1', 'a2']);
    my @suiteResults = ($suiteResult);
    my %reportData = Report::TestSessionReportBuilder::buildData(suiteResults => \@suiteResults, artifactsRoot => '/a/b');
    my @suites = @{ $reportData{suites} };
    my %suite = %{ $suites[0] };
    arraysAreEquivalent($suite{artifacts}, ['a1', 'a2']);
}

BuildData_SuiteResultWithOneTest_SerializedCorrectly:
{
    my $suiteResult = new SuiteResult(name => 'integration', platform => 'TestABV', artifacts => ['a1', 'a2']);
    my $tr = new TestResult();
    $tr->setTest('Test1');
    $tr->setFixture('ShaderTests');
    $tr->setTime('321');
    $tr->setState(TestResult::FAILURE);
    $tr->setMessage('Hej!');
    $tr->setStackTrace('file1: line1');
    $tr->setArtifacts(['a1', 'a2']);
    $suiteResult->addTestResult($tr);
    my @suiteResults = ($suiteResult);
    my %reportData = Report::TestSessionReportBuilder::buildData(suiteResults => \@suiteResults);
    my @suites = @{ $reportData{suites} };
    my %suite = %{ $suites[0] };
    my @tests = @{ $suite{tests} };
    is(scalar(@tests), 1);
    my %test = %{ $tests[0] };
    is($test{name}, 'Test1');
    is($test{fixture}, 'ShaderTests');
    is($test{time}, '321');
    is($test{state}, TestResult::FAILURE);
    is($test{message}, 'Hej!');
    is($test{reason}, undef);
    is($test{stackTrace}, 'file1: line1');
    arraysAreEquivalent(@{ $test{artifacts} }, ['a1', 'a2']);
}

BuildData_CollectorIsSpecified_SerializedCorrectly:
{
    my $collector1 = new Test::MockObject();
    $collector1->mock(name => sub { return 'collector1' });
    $collector1->mock(data => sub { return { a => '1' }; });

    my $collector2 = new Test::MockObject();
    $collector2->mock(name => sub { return 'collector2' });
    $collector2->mock(data => sub { return { return { b => 2 } }; });
    my @collectors = ($collector1, $collector2);

    my $suiteResult = new SuiteResult(name => 'integration', platform => 'TestABV');
    my @suiteResults = ($suiteResult);

    my %reportData = Report::TestSessionReportBuilder::buildData(suiteResults => \@suiteResults, collectors => \@collectors);
    is($reportData{collector1}{a}, '1');
    is($reportData{collector2}{b}, '2');
}

BuildData_TwoSuiteResults_SerializedCorrectly:
{
    my $suiteResult1 = new SuiteResult(name => 'integration', platform => 'TestABV');
    my $trFail = new TestResult();
    $trFail->setState(TestResult::FAILURE);
    $trFail->setMessage("Failed!");
    $suiteResult1->addTestResult($trFail);

    my $suiteResult2 = new SuiteResult(name => 'runtime', platform => 'editor');
    my $trSuccess = new TestResult();
    $trSuccess->setState(TestResult::SUCCESS);
    $suiteResult2->addTestResult($trSuccess);

    my @suiteResults = ($suiteResult1, $suiteResult2);
    my %reportData = Report::TestSessionReportBuilder::buildData(suiteResults => \@suiteResults);
    my @suites = @{ $reportData{suites} };
    my %suite1 = %{ $suites[0] };
    my %summarySuite1 = %{ $suite1{summary} };
    is($summarySuite1{success}, 0);

    my %suite2 = %{ $suites[1] };
    my %summarySuite2 = %{ $suite2{summary} };
    is($summarySuite2{success}, 1);

    my %topLevelSummary = %{ $reportData{summary} };
    is($topLevelSummary{success}, 0);
}

BuildData_SuiteResultWithOneIgnoredTestWithAReason_SerializedCorrectly:
{
    my $suiteResult = new SuiteResult(name => 'integration', platform => 'TestABV', artifacts => ['a1', 'a2']);
    my $tr = new TestResult();
    $tr->setTest('Test1');
    $tr->setFixture('ShaderTests');
    $tr->setTime('321');
    $tr->setState(TestResult::IGNORED);
    $tr->setReason('Ignored case 123');
    $tr->setArtifacts(['a1', 'a2']);
    $suiteResult->addTestResult($tr);
    my @suiteResults = ($suiteResult);
    my %reportData = Report::TestSessionReportBuilder::buildData(suiteResults => \@suiteResults);
    my @suites = @{ $reportData{suites} };
    my %suite = %{ $suites[0] };
    my @tests = @{ $suite{tests} };
    is(scalar(@tests), 1);
    my %test = %{ $tests[0] };
    is($test{name}, 'Test1');
    is($test{fixture}, 'ShaderTests');
    is($test{time}, '321');
    is($test{state}, TestResult::IGNORED);
    is($test{message}, undef);
    is($test{stackTrace}, undef);
    is($test{reason}, 'Ignored case 123');
    arraysAreEquivalent(@{ $test{artifacts} }, ['a1', 'a2']);
}

BuildData_OneSuite_MinimalCommandLineIsWrittenToTheSuiteResult:
{
    my $suiteResult = new SuiteResult(name => 'integration');
    my $trFail = new TestResult();
    $suiteResult->addTestResult($trFail);

    my @suiteResults = ($suiteResult);
    my %reportData = Report::TestSessionReportBuilder::buildData(
        suiteResults => \@suiteResults,
        commandLine => ['-suite=integration', '-env=monobe', '-testtarget=TestABV']
    );

    my @suites = @{ $reportData{suites} };
    my %suite = %{ $suites[0] };

    my $minimalCommandLine = $suite{minimalCommandLine};
    arraysAreEquivalent($minimalCommandLine, ['--environment=monobe', '--testtarget=TestABV']);
}

BuildData_OverriededOption_MinimalCommandLineIsWrittenToTheSuiteResult:
{
    my $suiteResult = new SuiteResult(name => 'editor');

    $suiteResult->addOptionOverride(name => 'projectpath', value => 'project/1');

    my $trFail = new TestResult();
    $suiteResult->addTestResult($trFail);

    my @overridedOptions = $suiteResult->getOverridedOptions();
    my @suiteResults = ($suiteResult);
    my %reportData = Report::TestSessionReportBuilder::buildData(
        suiteResults => \@suiteResults,
        commandLine => ['-suite=editor', '-projectpath=project'],
        overridedOptions => \@overridedOptions
    );

    my @suites = @{ $reportData{suites} };
    my %suite = %{ $suites[0] };
    my $minimalCommandLine = $suite{minimalCommandLine};
    arraysAreEquivalent($minimalCommandLine, ['--projectpath=project/1']);
}

BuildData_SuiteResultContainsAssociatedData_AddsItToReportData:
{
    my $suiteResult = new SuiteResult(name => 'integration', platform => 'TestABV', artifacts => ['/a/b/a1', 'a2']);
    $suiteResult->addData('key_name', 'data');
    my @suiteResults = ($suiteResult);
    my %reportData = Report::TestSessionReportBuilder::buildData(suiteResults => \@suiteResults, artifactsRoot => '/a/b');
    my @suites = @{ $reportData{suites} };
    my %suite = %{ $suites[0] };
    is($suite{data}->{'key_name'}, 'data');
}

BuildData_SuiteContainsFailureReasons_AddsItToReportData:
{
    my $suiteResult = new SuiteResult(name => 'integration', platform => 'TestABV', artifacts => ['/a/b/a1', 'a2']);
    $suiteResult->forceFailed("failure message");
    my @suiteResults = ($suiteResult);
    my %reportData = Report::TestSessionReportBuilder::buildData(suiteResults => \@suiteResults, artifactsRoot => '/a/b');
    my @suites = @{ $reportData{suites} };
    my %suite = %{ $suites[0] };
    is($suite{forciblyFailed}, 1);
    arraysAreEqual($suite{failureReasons}, ['failure message']);
}

BuildData_TestResultContainsAssociatedData_AddsItToReportData:
{
    my $suiteResult = new SuiteResult(name => 'integration');
    my $tr = new TestResult();
    $tr->addData('key_name', 'data');
    $suiteResult->addTestResult($tr);

    my @suiteResults = ($suiteResult);
    my %reportData = Report::TestSessionReportBuilder::buildData(
        suiteResults => \@suiteResults,
        commandLine => ['-suite=integration']
    );

    my @suites = @{ $reportData{suites} };
    my %suite = %{ $suites[0] };

    my @tests = @{ $suite{tests} };
    my %test = %{ $tests[0] };
    is($test{data}->{'key_name'}, 'data');
}

BuildData_SuiteResultContainsDetails_AddsItToReportData:
{
    my $suiteResult = new SuiteResult(name => 'integration');
    $suiteResult->setDetails('longer suite descripton');
    my @suiteResults = ($suiteResult);
    my %reportData = Report::TestSessionReportBuilder::buildData(
        suiteResults => \@suiteResults,
        commandLine => ['-suite=integration']
    );

    my @suites = @{ $reportData{suites} };
    my %suite = %{ $suites[0] };
    is($suite{details}, 'longer suite descripton');
}

BuildData_SavesPathToTestResultXml:
{
    my $suiteResult = new SuiteResult(name => 'integration');
    $suiteResult->setDetails('longer suite descripton');
    $suiteResult->setTestResultXmlFilePath('path_to_xml');
    my %reportData = Report::TestSessionReportBuilder::buildData(suiteResults => [$suiteResult]);

    my @suites = @{ $reportData{suites} };
    my %suite = %{ $suites[0] };
    is($suite{testresultsxml}, 'path_to_xml');
}

BuildData_SavesDescription:
{
    my $suiteResult = new SuiteResult(name => 'integration');
    $suiteResult->setDescription('descripton text');
    my %reportData = Report::TestSessionReportBuilder::buildData(suiteResults => [$suiteResult]);

    my @suites = @{ $reportData{suites} };
    my %suite = %{ $suites[0] };
    is($suite{description}, 'descripton text');
}

done_testing();
