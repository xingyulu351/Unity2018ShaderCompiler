use warnings;
use strict;

use Dirs;
use lib Dirs::external_root();

use File::Spec::Functions qw (canonpath);
use TestHelpers::Runners qw (runAndGrabExecuteInput listAndGrabExecuteInput makeDefaultSuiteRunnerArtifactNameBuilderStub);
use TestHelpers::ArrayAssertions qw (arraysAreEqual arrayStartsWith arrayContains arrayEndsWith);
use SuiteRunners::PerformanceRuntime;
use TestHelpers::Runners;
use Dirs;
use lib Dirs::external_root();
use Array::Utils qw (unique);
use Test::More;
use Test::Deep 're';
use Test::MockModule;

my ($root) = TestHelpers::Runners::getFileNames(artifacts => ['root']);

my $runnerOptionsLoaderMock = new Test::MockModule('RunnerOptions');
$runnerOptionsLoaderMock->mock(readFromFile => sub { return (); });

my $runtimePerfTestRunner = 'test_runner_mocked_name';
my $testAssemblyMockedName = 'Debug/bin/test_assembly_mocked_name';
my $extensionsSearchPaths = FileUtils::getDirName($testAssemblyMockedName);

my $performanceReporterMock = new Test::MockModule('PerformanceReporter');
$performanceReporterMock->mock(report => sub { });

Run_RequiredArguments_ProducesCorrectCommandLineAndArtifacts:
{
    my $runner = makeRunner();

    my $nameBuilderStub = makeDefaultSuiteRunnerArtifactNameBuilderStub();

    $nameBuilderStub->specifyExpectation(
        method => 'getFullAtrifactFileName',
        params => { 'parent-type' => 'Unity.PerformanceTests.RuntimeTestRunner', type => 'TestResults', extension => 'xml' },
        result => 'test_results_xml_full_file_name'
    );

    $nameBuilderStub->specifyExpectation(
        method => 'getFullAtrifactFileName',
        params => { 'parent-type' => 'Unity.PerformanceTests.RuntimeTestRunner', type => 'stdout', extension => 'txt' },
        result => 'stdout_full_path'
    );

    $nameBuilderStub->specifyExpectation(
        method => 'getFullAtrifactFileName',
        params => { 'parent-type' => 'Unity.PerformanceTests.RuntimeTestRunner', type => 'stderr', extension => 'txt' },
        result => 'stderr_full_path'
    );

    $nameBuilderStub->specifyExpectation(
        method => 'getFullAtrifactFileName',
        params => { 'parent-type' => 'Unity.PerformanceTests.RuntimeTestRunner', type => 'perfdata', extension => 'json' },
        result => 'perf_data_json_full_path'
    );

    $nameBuilderStub->specifyExpectation(
        method => 'getFullAtrifactFileName',
        params => { 'parent-type' => 'UnifiedTestRunner', type => 'TestPlan', extension => 'txt' },
        result => 'test_plan_full_file_name'
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
    is(scalar(@input), 10);
    arrayContains(
        \@input,
        [
            $runtimePerfTestRunner, '--platform=editor',
            '--testresultsxml=test_results_xml_full_file_name', "--assemblywithtests=$testAssemblyMockedName",
            "--artifactspath=" . $runner->getArtifactsPath(), "--extensionssearchpaths=$extensionsSearchPaths",
        ]
    );
    arrayEndsWith(\@input, ['1>stdout_full_path', '2>stderr_full_path']);

    my @artifacts = $runner->getArtifacts()->get();

    arraysAreEqual(\@artifacts, ['stdout_full_path', 'stderr_full_path', 'path_to_reporter_stdout', 'path_to_reporter_stderr']);
}

Run_PlatformIsSpecified_PlaceItInResultCommandLine:
{
    my $runner = makeRunner();
    my @input = runAndGrabExecuteInput($runner, '--platform=standalone');
    arrayContains(\@input, '--platform=standalone');
}

Run_TestResultXmlIsSpecified_PlaceItInResultCommandLine:
{
    my $runner = makeRunner();
    my @input = runAndGrabExecuteInput($runner, '--testresultsxml=c:\\a\\b.XML');
    arrayContains(\@input, ["--testresultsxml=" . canonpath("c:\\a\\b.XML")]);
}

Run_FilterIsSpecified_PlacedItInResultCommandLine:
{
    my $runner = makeRunner();
    my @input = runAndGrabExecuteInput($runner, '--testfilter=something');
    arrayContains(\@input, '--testfilter=something');
}

Run_DefaultArguments_WatchingStdOutAndStdErr:
{
    my $runner = makeRunner();

    runAndGrabExecuteInput($runner);

    my $system = $runner->getEnvironment()->getSystemCall();

    my @filesToWatch = $system->getFilesToWatch();
    my @files = unique(map { $_->{file} } @filesToWatch);
    arrayContains(\@files, [re(qr/stdout[.]txt/), re(qr/stderr[.]txt/)]);
}

List_DefaultArguments_PlacesExpectedArgumentsInsideResultCommandLine:
{
    my $runner = makeRunner();
    my @runnerArgs = ('--list');
    my @input = listAndGrabExecuteInput($runner, @runnerArgs);
    my $assemlyWithTests = canonpath("$root/Tests/bin/Debug/Unity.RuntimeTests.dll");
    arrayContains(\@input, [$runtimePerfTestRunner, '--list', '--platform=editor', "--assemblywithtests=$testAssemblyMockedName", re(qr />.*TestPlan[.]txt/)]);
}

Run_RunnerHasOneFlagOptionInRunnerOptionsFile_WillPassItToRunner:
{

    my $option = new Option(
        names => ['flag-option'],
        valueType => Option->ValueTypeNone
    );

    my $optionsReader = new Test::MockModule('RunnerOptions');
    $optionsReader->mock(readFromFile => sub { return ($option); });
    my $runner = makeRunner();

    my @input = runAndGrabExecuteInput($runner, '--flag-option');

    arrayContains(\@input, '--flag-option');
}

List_RunnerHasOneFlagOptionInRunnerOptionsFile_WillPassItToRunner:
{
    my $optionsReader = new Test::MockModule('RunnerOptions');

    my $option = new Option(
        names => ['flag-option'],
        valueType => Option->ValueTypeNone
    );

    $optionsReader->mock('read', sub { return ($option); });

    my $runner = makeRunner();

    my @input = listAndGrabExecuteInput($runner, '--flag-option');

    arrayContains(\@input, '--flag-option');
}

InvokesBaseClassWithProperConstructor:
{
    my $runnerOptionsLoaderMock = new Test::MockModule('Plugin');
    my %args;
    $runnerOptionsLoaderMock->mock(new => sub { (undef, %args) = @_; });
    my $runner = new SuiteRunners::PerformanceRuntime();
    my $runnerProj = canonpath("$root/Tests/Unity.PerformanceTests.RuntimeTestRunner/Unity.PerformanceTests.RuntimeTestRunner.csproj");
    is($args{optionsGenProj}, $runnerProj);
}

Run_TestListIsSpecified_DoesNotBuildTheTestPlan:
{
    my $runner = makeRunner();

    my @input = runAndGrabExecuteInput($runner, '--testlist=path_to_testlist');

    arrayContains(\@input, '--testlist=path_to_testlist');
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

sub makeRunner
{
    my $runner = new SuiteRunners::PerformanceRuntime();
    my $runContext = RunContext::new(new Options(options => $runner->{suiteOptions}));
    $runner->setRunContext($runContext);
    $runner = new Test::MockObject::Extends($runner);
    $runner->mock(writeSuiteData => sub { });
    $runner->mock(_generateMSBuildScript => sub { return 'msbuild_file_name' });
    $runner->mock(_getAssemblyFullPath => sub { return $runtimePerfTestRunner; });
    $runner->mock(_getOutputBuildPath => sub { return 'build_path'; });
    $runner->mock(_getTestAssemblyFullPath => sub { return $testAssemblyMockedName; });
    return $runner;
}
