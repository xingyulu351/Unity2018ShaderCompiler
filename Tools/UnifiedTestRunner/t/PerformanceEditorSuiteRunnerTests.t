use warnings;
use strict;

use File::Spec::Functions qw (canonpath);
use TestHelpers::Runners qw (runAndGrabExecuteInput listAndGrabExecuteInput makeDefaultSuiteRunnerArtifactNameBuilderStub);
use TestHelpers::ArrayAssertions qw (arraysAreEqual arrayDoesNotContain arrayContains arraysAreEquivalent arrayEndsWith);
use SuiteRunners::PerformanceEditor;
use TestHelpers::Runners;
use Dirs;
use lib Dirs::external_root();
use Array::Utils qw (unique);
use Test::More;
use TestHelpers::Stub;
use Test::MockObject::Extends;
use Test::Deep 're';
use Test::MockModule;

my ($mono, $root) = TestHelpers::Runners::getFileNames(artifacts => ['mono', 'root']);

my $performanceEditorSuiteRunner = 'test_runner_mocked_name';
my $testAssemblyMockedName = 'test_assembly_mocked_name';

my $runnerOptionsLoaderMock = new Test::MockModule('RunnerOptions');
$runnerOptionsLoaderMock->mock(readFromFile => sub { return (); });

my $performanceReporterMock = new Test::MockModule('PerformanceReporter');
$performanceReporterMock->mock(report => sub { });

Run_RequiredArgs_ProducesCorrectCommandLineAndArtifactsList:
{
    my $runner = makeRunner();
    my $nameBuilderStub = makeDefaultSuiteRunnerArtifactNameBuilderStub();

    $nameBuilderStub->specifyExpectation(
        method => 'getFullAtrifactFileName',
        params => { 'parent-type' => 'Unity.PerformanceTests.Runner', type => 'stdout', extension => 'txt' },
        result => 'stdout_full_path'
    );

    $nameBuilderStub->specifyExpectation(
        method => 'getFullAtrifactFileName',
        params => { 'parent-type' => 'Unity.PerformanceTests.Runner', type => 'stderr', extension => 'txt' },
        result => 'stderr_full_path'
    );

    $nameBuilderStub->specifyExpectation(
        method => 'getFullAtrifactFileName',
        params => { 'parent-type' => 'Unity.PerformanceTests.Runner', type => 'TestResults', extension => 'xml' },
        result => 'test_results_xml_full_file_name'
    );

    $nameBuilderStub->specifyExpectation(
        method => 'getFullAtrifactFileName',
        params => { 'parent-type' => 'Unity.PerformanceTests.Runner', type => 'perfdata', extension => 'json' },
        result => 'perf_data_json_full_path'
    );

    $nameBuilderStub->specifyExpectation(
        method => 'getFullFolderPathForArtifactType',
        params => { type => 'Artifacts' },
        result => 'path_to_unity_artifacts_root'
    );

    $nameBuilderStub->specifyExpectation(
        method => 'getFullAtrifactFileName',
        params => { 'parent-type' => 'Unity.PerformanceTests.Reporter', type => 'stdout', extension => 'txt' },
        result => 'path_to_reporter_stdout'
    );

    $nameBuilderStub->specifyExpectation(
        method => 'getFullAtrifactFileName',
        params => { 'parent-type' => 'Unity.PerformanceTests.Reporter', type => 'stderr', extension => 'txt' },
        result => 'path_to_reporter_stderr'
    );

    $runner->setArtifactsNameBuilder($nameBuilderStub->object());

    my @input = runAndGrabExecuteInput($runner);
    my $assemblyWithTests = canonpath("$root/Tests/bin/Debug/Unity.PerformanceTests.dll");
    arraysAreEqual(
        \@input,
        [
            $mono, '--debug', $performanceEditorSuiteRunner,
            "--artifactspath=path_to_unity_artifacts_root",
            '--testresultsxml=test_results_xml_full_file_name',
            "--assemblywithtests=test_assembly_mocked_name",
            '1>stdout_full_path', '2>stderr_full_path'
        ]
    );

    my @artifacts = $runner->getArtifacts()->get();
    arraysAreEquivalent(\@artifacts,
        ['stdout_full_path', 'stderr_full_path', 'path_to_unity_artifacts_root', 'path_to_reporter_stdout', 'path_to_reporter_stderr']);
}

Run_TestResultsXml_ProducesCorrectCommandLineAndArtifactsList:
{
    my $runner = makeRunner();
    my @runnerArgs = ('--testresultsxml=123.xml');
    my @input = runAndGrabExecuteInput($runner, @runnerArgs);
    my @artifacts = $runner->getArtifacts()->get();
    arrayDoesNotContain(\@artifacts, '123.xml');
}

Run_RunnerHasOneFlagOptionInRunnerOptionsFile_WillPassItToRunner:
{
    my $optionsReader = new Test::MockModule('RunnerOptions');

    my $option = new Option(
        names => ['flag-option'],
        valueType => Option->ValueTypeNone
    );

    $optionsReader->mock(readFromFile => sub { return ($option); });
    my $runner = makeRunner();

    my @input = runAndGrabExecuteInput($runner, '--flag-option');

    arrayContains(\@input, '--flag-option');
}

List_RequiredArguments_BuildsCorrectCommandLine:
{
    my $runner = makeRunner();

    my $nameBuilderStub = makeDefaultSuiteRunnerArtifactNameBuilderStub();
    $nameBuilderStub->specifyExpectation(
        method => 'getFullAtrifactFileName',
        params => { 'parent-type' => 'UnifiedTestRunner', type => 'TestPlan', extension => 'txt' },
        result => 'test_plan_full_file_name'
    );

    $runner->setArtifactsNameBuilder($nameBuilderStub->object());

    my $protocolModuleMock = new Test::MockModule('UnityTestProtocol::Utils');
    $protocolModuleMock->mock(getFirst => sub { return undef; });

    my @input = listAndGrabExecuteInput($runner, '--list');
    arrayEndsWith(\@input, [$performanceEditorSuiteRunner, '--list', '--assemblywithtests=test_assembly_mocked_name', '1>test_plan_full_file_name']);
}

List_RequiredArguments_BuildsCorrectCommandLine:
{
    my $runner = makeRunner();

    my $nameBuilderStub = makeDefaultSuiteRunnerArtifactNameBuilderStub();
    $nameBuilderStub->specifyExpectation(
        method => 'getFullAtrifactFileName',
        params => { 'parent-type' => 'UnifiedTestRunner', type => 'TestPlan', extension => 'txt' },
        result => 'test_plan_full_file_name'
    );

    $runner->setArtifactsNameBuilder($nameBuilderStub->object());

    my $protocolModuleMock = new Test::MockModule('UnityTestProtocol::Utils');
    $protocolModuleMock->mock(getFirst => sub { return undef; });

    my @input = listAndGrabExecuteInput($runner, '--list', '--testfilter=abc');
    my $assemblyWithTests = canonpath("$root/Tests/bin/Debug/Unity.PerformanceTests.dll");
    arrayEndsWith(\@input,
        [$performanceEditorSuiteRunner, '--list', '--testfilter=abc', '--assemblywithtests=test_assembly_mocked_name', '1>test_plan_full_file_name']);
}

InvokesBaseClassWithProperConstructor:
{
    my $runnerOptionsLoaderMock = new Test::MockModule('Plugin');
    my %args;
    $runnerOptionsLoaderMock->mock(new => sub { (undef, %args) = @_; });
    my $runner = new SuiteRunners::PerformanceEditor();
    my $runnerProj = canonpath("$root/Tests/Unity.PerformanceTests.Runner/Unity.PerformanceTests.Runner.csproj");
    is($args{optionsGenProj}, $runnerProj);
}

Invokes_reportPerformanceData:
{
    my $runner = makeRunner();
    my $invoked = 0;
    $runner->mock(
        _reportPerformanceData => sub
        {
            $invoked = 1;
        }
    );
    runAndGrabExecuteInput($runner);
    is($invoked, 1);
}

done_testing();

sub run
{
    my (@args) = @_;
    my $runner = makeRunner();
    return runAndGrabExecuteInput($runner, @args);
}

sub makeRunner
{
    my $runner = new SuiteRunners::PerformanceEditor();
    my $nameBuilderStub = makeDefaultSuiteRunnerArtifactNameBuilderStub();
    $runner->setArtifactsNameBuilder($nameBuilderStub->object());
    $runner = new Test::MockObject::Extends($runner);
    $runner->mock(writeTestResultXml => sub { });
    $runner->mock(writeSuiteData => sub { });
    $runner->mock(_generateMSBuildScript => sub { return 'msbuild_file_name' });
    $runner->mock(_getAssemblyFullPath => sub { return $performanceEditorSuiteRunner; });
    $runner->mock(_getOutputBuildPath => sub { return 'build_path'; });
    $runner->mock(_updateRunnerHelp => sub { return 'Options.generated.json' });
    $runner->mock(_getTestAssemblyFullPath => sub { return $testAssemblyMockedName; });

    return $runner;
}
