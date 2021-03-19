use warnings;
use strict;

use Test::More;

use TestHelpers::Runners;
use SuiteRunners::Native;
use TestHelpers::Runners qw (runAndGrabExecuteInput runAndGrabBuildEngineInput  listAndGrabExecuteInput makeDefaultSuiteRunnerArtifactNameBuilderStub);
use TestHelpers::ArrayAssertions qw (arraysAreEqual arrayContains arraysAreEquivalent arrayDoesNotContain arrayStartsWith);
use TestHelpers::Stub;
use Dirs;

use File::Spec::Functions qw (canonpath);
use lib Dirs::external_root();
use Test::MockObject::Extends;
use Test::Deep 're';
use Test::MockModule;
use ArtifactsMapBuilder;
my ($mono, $root) = TestHelpers::Runners::getFileNames(artifacts => ['mono', 'root']);

my $artifactsMapBuilderMock = new Test::MockModule('ArtifactsMapBuilder');
$artifactsMapBuilderMock->mock(fromNativeSuiteLog => sub { return new ArtifactsMap(); });

my $performanceReporterMock = new Test::MockModule('PerformanceReporter');
$performanceReporterMock->mock(report => sub { });

Run_RequiredArgs_ProducesCorrectCommandLineAndArtifactsList:
{
    my $fileUtilsMock = new Test::MockModule('FileUtils');

    #assume all files exist
    $fileUtilsMock->mock(fileExists => sub { return 1; });

    my $runner = makeRunner();
    my $nameBuilderStub = makeDefaultSuiteRunnerArtifactNameBuilderStub();

    $nameBuilderStub->specifyExpectation(
        method => 'getFullAtrifactFileName',
        params => { 'parent-type' => 'Unity', type => 'log', extension => 'txt' },
        result => 'full_path_to_unity_log_file'
    );

    $nameBuilderStub->specifyExpectation(
        method => 'getFullAtrifactFileName',
        params => { 'parent-type' => 'NativeTestsRunner', type => 'TestResults', extension => 'xml' },
        result => 'test_results_xml_full_file_name'
    );

    $nameBuilderStub->specifyExpectation(
        method => 'getFullAtrifactFileName',
        params => { 'parent-type' => 'NativeTestsRunner', type => 'stdout', extension => 'txt' },
        result => 'stdout_full_path'
    );

    $nameBuilderStub->specifyExpectation(
        method => 'getFullAtrifactFileName',
        params => { 'parent-type' => 'NativeTestsRunner', type => 'stderr', extension => 'txt' },
        result => 'stderr_full_path'
    );

    $runner->setArtifactsNameBuilder($nameBuilderStub->object());
    my @input = runAndGrabExecuteInput($runner);

    arraysAreEqual(
        \@input,
        [
            $mono, '--debug',
            re('Unity[.]NativeTestsRunner[.]exe'), '--platform=editor',
            '--category=Unit', '--category=Integration',
            '--testresultsxml=test_results_xml_full_file_name', '--logfile=full_path_to_unity_log_file',
            '--artifactspath=' . $runner->getArtifactsPath(), '1>stdout_full_path',
            '2>stderr_full_path'
        ]
    );

    my @artifacts = $runner->getArtifacts()->get();
    arraysAreEquivalent(\@artifacts, ['full_path_to_unity_log_file', 'stdout_full_path', 'stderr_full_path']);
}

Run_EditorLogDoesNotExist_DoesNotRegisterArtifact:
{
    my $runner = makeRunner();
    my $nameBuilderStub = makeDefaultSuiteRunnerArtifactNameBuilderStub();

    $nameBuilderStub->specifyExpectation(
        method => 'getFullAtrifactFileName',
        params => { 'parent-type' => 'Unity', type => 'log', extension => 'txt' },
        result => 'full_path_to_unity_log_file'
    );

    $runner->setArtifactsNameBuilder($nameBuilderStub->object());
    my $fileUtilsMock = new Test::MockModule('FileUtils');
    $fileUtilsMock->mock(fileExists => sub { return 0; });

    my @input = runAndGrabExecuteInput($runner, '--platform=standalone');

    my @artifacts = $runner->getArtifacts()->get();
    arrayDoesNotContain(\@artifacts, 'full_path_to_unity_log_file');
}

Run_RespectsPerformanceCateogory:
{
    my @input = run('--category=performance');
    arrayContains(\@input, [re('Unity.NativeTestsRunner.exe')]);
    arrayDoesNotContain(\@input, [re(qr/=integration/i), re(qr/=stress/i), re(qr/=regression/i)]);
}

Invokes_ReportPerformanceData:
{
    my $runner = makeRunner();

    my $performanceReporterMock = new Test::MockModule('PerformanceReporter');
    $performanceReporterMock->mock(ShouldReportPerformanceData => sub { return 1; });

    my $invoked = 0;
    $runner->mock(
        _reportPerformanceData => sub
        {
            $invoked = 1;
        }
    );
    runAndGrabExecuteInput($runner, '--category=performance');
    is($invoked, 1);
}

DoesNotInvoke_DoNotReportRepfrmanceData:
{
    my $runner = makeRunner();
    my $performanceReporterMock = new Test::MockModule('PerformanceReporter');
    $performanceReporterMock->mock(ShouldReportPerformanceData => sub { return 0; });
    my $invoked = 0;
    $runner->mock(
        _reportPerformanceData => sub
        {
            $invoked = 1;
        }
    );
    runAndGrabExecuteInput($runner, '--category=stress');
    is($invoked, 0);
}

Run_LogFileIsSpecified_ProducesCorrectCommandLineAndArtifactsList:
{
    my @input = run('--logfile=path_to_unity_log_file');
    arrayContains(\@input, '--logfile=path_to_unity_log_file');
}

Run_LogFileIsSpecified_ProducesCorrectCommandLineAndArtifactsList:
{
    my @input = run('--logfile=path_to_unity_log_file');
    arrayContains(\@input, '--logfile=path_to_unity_log_file');
}

DefaultBuildModeIsMinimal:
{
    my $runner = makeRunner();
    is($runner->getOptionValue('build-mode'), 'minimal');
}

BuildIsolationIsEnabledByDefault:
{
    my $runner = makeRunner();
    is($runner->getOptionValue('build-isolation'), 'enabled');
}

Run_TestResultsXml_ProducesCorrectCommandLineAndArtifactsList:
{
    my $runner = makeRunner();
    my @runnerArgs = ('--testresultsxml=123.xml');
    my @input = runAndGrabExecuteInput($runner, @runnerArgs);
    my @artifacts = $runner->getArtifacts()->get();
    arrayDoesNotContain(\@artifacts, '123.xml');
}

InvokesBaseClassWithProperConstructor:
{
    my $runnerOptionsLoaderMock = new Test::MockModule('Plugin');
    my %args;
    $runnerOptionsLoaderMock->mock(new => sub { (undef, %args) = @_; });
    my $runner = new SuiteRunners::Native();
    my $nativeRunnerProj = canonpath("$root/Tests/Unity.NativeTestsRunner/Unity.NativeTestsRunner.csproj");
    is($args{optionsGenProj}, $nativeRunnerProj);
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

Run_ArtifactsMapIsProduced_RegisterItAsArtifacts:
{
    my $runner = makeRunner();
    my $artifactsMapBuilderMock = new Test::MockModule('ArtifactsMapBuilder');
    $artifactsMapBuilderMock->mock(fromNativeSuiteLog => sub { return new ArtifactsMap(); });

    $artifactsMapBuilderMock->mock(
        fromNativeSuiteLog => sub
        {
            my $result = new ArtifactsMap();
            $result->addArtifact('player.log');
            return $result;
        }
    );

    runAndGrabExecuteInput($runner);

    my @artifacts = $runner->getArtifacts()->get();
    arrayContains(\@artifacts, 'player.log');
}

List_RequiredArguments_BuildsCorrectCommandLine:
{
    my $graphicsTestsRunnerMock = 'test_runner_mocked_name';

    my $runner = makeRunner();

    my $nameBuilderStub = makeDefaultSuiteRunnerArtifactNameBuilderStub();
    $nameBuilderStub->specifyExpectation(
        method => 'getFullAtrifactFileName',
        params => { 'parent-type' => 'UnifiedTestRunner', type => 'TestPlan', extension => 'txt' },
        result => 'test_plan_full_file_name'
    );

    $nameBuilderStub->specifyExpectation(
        method => 'getFullAtrifactFileName',
        params => { 'parent-type' => 'Unity', type => 'log', extension => 'txt' },
        result => 'full_path_to_unity_log_file'
    );

    $nameBuilderStub->specifyExpectation(
        method => 'getFullAtrifactFileName',
        params => { 'parent-type' => 'NativeTestsRunner', type => 'stdout', extension => 'txt' },
        result => 'stdout_full_path'
    );

    $nameBuilderStub->specifyExpectation(
        method => 'getFullAtrifactFileName',
        params => { 'parent-type' => 'NativeTestsRunner', type => 'stderr', extension => 'txt' },
        result => 'stderr_full_path'
    );

    $runner->setArtifactsNameBuilder($nameBuilderStub->object());

    my $protocolModuleMock = new Test::MockModule('UnityTestProtocol::Utils');
    $protocolModuleMock->mock(getFirst => sub { return undef; });

    my @input = listAndGrabExecuteInput($runner);
    arraysAreEqual(
        \@input,
        [
            $mono, '--debug', re('Unity[.]NativeTestsRunner[.]exe'),
            '--list', '--platform=editor', '--category=Unit', '--category=Integration', '--logfile=full_path_to_unity_log_file',
            '1>stdout_full_path', '2>stderr_full_path',
        ]
    );
}

List_AddsFilterToTheCommandListIfSpecified:
{
    my $runner = makeRunner();
    my @input = listAndGrabExecuteInput($runner, '--testfilter=abc');
    arrayContains(\@input, ['--list', '--testfilter=abc']);
}

List_AddsCategoryToTheCommandLinstIfSpecified:
{
    my $runner = makeRunner();
    my @input = listAndGrabExecuteInput($runner, '--category=performance');
    arrayContains(\@input, ['--list', '--category=performance']);
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
    my $runner = new SuiteRunners::Native();
    my $buildEngine = new BuildEngines::XBuild();
    my $nameBuilderStub = makeDefaultSuiteRunnerArtifactNameBuilderStub();
    $runner->setArtifactsNameBuilder($nameBuilderStub->object());
    $runner = new Test::MockObject::Extends($runner);
    my $runContext = RunContext::new(new Options(options => $runner->{suiteOptions}));
    $runner->setRunContext($runContext);
    $runner->mock(writeTestResultXml => sub { });
    $runner->mock(writeSuiteData => sub { });
    $runner->mock(_generateMSBuildScript => sub { return 'msbuild_file_name' });
    $runner->mock(_getOutputBuildPath => sub { return 'build_path'; });
    $runner->mock(cleanCSharpProjectsSln => sub { });
    return $runner;
}
