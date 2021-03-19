use warnings;
use strict;

use Test::More;

use SuiteRunners::Graphics;
use TestHelpers::Runners;
use TestHelpers::Runners
    qw (runAndGrabExecuteInput runAndGrabBuildEngineInput listAndGrabExecuteInput makeDefaultSuiteRunnerArtifactNameBuilderStub buildPlayerAndGrabExecuteInput);
use TestHelpers::ArrayAssertions qw (arraysAreEqual arrayStartsWith arrayContains arrayDoesNotContain arrayEndsWith arraysAreEquivalent isEmpty);
use File::Spec::Functions qw (canonpath);
use lib Dirs::external_root();
use Test::MockObject::Extends;
use Test::MockModule;
use Environments::Mono;
use Test::Deep 're';
use Test::Trap;
use ResultCollector;

my ($mono, $root) = TestHelpers::Runners::getFileNames(artifacts => ['mono', 'root']);

my $artifactsMapBuilderMock = new Test::MockModule('ArtifactsMapBuilder');
$artifactsMapBuilderMock->mock(fromGraphicsTestRunnerStdout => sub { return new ArtifactsMap(); });

my $unifiedTestRunnerCSharpProj = canonpath("$root/Tests/Unity.UnifiedTestRunner/Unity.UnifiedTestRunner.csproj");

my $graphicsTestsRunnerMock = 'test_runner_mocked_name';

my $fileUtilsMock = new Test::MockModule('FileUtils');
$fileUtilsMock->mock(readJsonObj => sub { return {} });

my @requiredArgs = ('--suite=graphics');

Run_RequiredArguments_ProducesCorrectCommandLineAndArtifacts:
{
    my $runner = makeRunner();

    my $nameBuilderStub = makeDefaultSuiteRunnerArtifactNameBuilderStub();

    $nameBuilderStub->specifyExpectation(
        method => 'getFullAtrifactFileName',
        params => { 'parent-type' => 'GraphicsTestsRunner', type => 'stdout', extension => 'txt' },
        result => 'stdout_full_path'
    );

    $nameBuilderStub->specifyExpectation(
        method => 'getFullAtrifactFileName',
        params => { 'parent-type' => 'GraphicsTestsRunner', type => 'stderr', extension => 'txt' },
        result => 'stderr_full_path'
    );

    $nameBuilderStub->specifyExpectation(
        method => 'getFullAtrifactFileName',
        params => { 'parent-type' => 'UnifiedTestRunner', type => 'TestPlan', extension => 'txt' },
        result => 'test_plan_full_file_name'
    );

    $nameBuilderStub->specifyExpectation(
        method => 'getFullFolderPathForArtifactType',
        params => { type => 'Results' },
        result => 'path_to_results_folder'
    );

    $nameBuilderStub->specifyExpectation(
        method => 'getFullFolderPathForArtifactType',
        params => { type => 'Players' },
        result => 'path_to_built_players_root'
    );

    $nameBuilderStub->specifyExpectation(
        method => 'getFullFolderPathForArtifactType',
        params => { type => 'Artifacts' },
        result => 'path_to_unity_artifacts_root'
    );

    $nameBuilderStub->specifyExpectation(
        method => 'getFullAtrifactFileName',
        params => { 'parent-type' => 'GraphicsTestsRunner', type => 'FailedImages', extension => 'zip' },
        result => 'failed_images_zip'
    );

    $nameBuilderStub->specifyExpectation(
        method => 'getSuiteRunRoot',
        result => '/suite_run_root'
    );

    $runner->setArtifactsNameBuilder($nameBuilderStub->object());

    my @input = runAndGrabExecuteInput($runner, @requiredArgs);

    arrayStartsWith(\@input, [$mono, '--debug', $graphicsTestsRunnerMock, '--suite=graphics']);
    arrayContains(\@input, '--resultsfolder=path_to_results_folder');
    arrayEndsWith(\@input, ['--no-stdout']);
}

Run_Platform_PutsItIntoResultCommandLine:
{
    my @input = run('--platform=XBOX360');
    arrayContains(\@input, '--platform=XBOX360');
}

Run_Configuration_PutsItIntoResultCommandLine:
{
    my @input = run('--configuration=dx584');
    arrayContains(\@input, '--configuration=dx584');
}

Run_Filter_PutsItIntoResultCommandLine:
{
    my @input = run('--testfilter=Skybox');
    arrayContains(\@input, '--testfilter=Skybox');
}

Run_Device_PutsItIntoResultCommandLine:
{
    my @input = run('--device=androidx');
    arrayContains(\@input, '--device=androidx');
}

Run_ResultsFolder_PutsItIntoResultCommandLine:
{
    my @input = run('--resultsfolder=FolderToStoreImageResults');
    arrayContains(\@input, '--resultsfolder=FolderToStoreImageResults');
}

Run_PlayerLoadPath_PlacesPlayerLoadPathIntoTheCommandLine:
{
    my $runner = makeRunner();
    $runner = new Test::MockObject::Extends($runner);
    $runner->mock(readTestPlanFromPlayerBuild => sub { return new TestPlan(); });
    my @input = runAndGrabExecuteInput($runner, '--player-load-path=players_load_path');
    arrayContains(\@input, ["--player-load-path=players_load_path"]);
    arrayDoesNotContain(\@input, qr/--player-save-path/);
}

BuildPlayer_InvokesBuildAuotmationDependencies:
{
    my $runner = makeRunner();
    $runner = new Test::MockObject::Extends($runner);

    my $automationPlayer = new Test::MockModule('AutomationPlayer');
    my $invokedWithPlatforms;
    $automationPlayer->mock(
        resolve => sub
        {
            my ($platform) = @_;
            $invokedWithPlatforms = $platform;
            return 'Platform1.Player.csproj';
        }
    );

    my $fileWatcher = new Test::MockModule('FileWatcher');
    $fileWatcher->mock(watch => sub { });
    $fileWatcher->mock(start => sub { });
    $fileWatcher->mock(stop => sub { });

    my %buildEngineInput = runAndGrabBuildEngineInput($runner, '--build-only', '--platform=platform_name', '--testfilter=001');

    is($invokedWithPlatforms, 'platform_name');
    is($buildEngineInput{project}, 'Platform1.Player.csproj');
}

Run_BuildPlayerOnly_PlacesItToTheCommandLine:
{
    my $runner = makeRunner();

    my $nameBuilderStub = makeDefaultSuiteRunnerArtifactNameBuilderStub();

    $nameBuilderStub->specifyExpectation(
        method => 'getFullFolderPathForArtifactType',
        params => { type => 'Players' },
        result => 'path_to_built_players_root'
    );

    $nameBuilderStub->specifyExpectation(
        method => 'getFullAtrifactFileName',
        params => { 'parent-type' => 'GraphicsTestsRunner', type => 'stdout', extension => 'txt' },
        result => 'stdout_full_path'
    );

    $nameBuilderStub->specifyExpectation(
        method => 'getFullAtrifactFileName',
        params => { 'parent-type' => 'GraphicsTestsRunner', type => 'stderr', extension => 'txt' },
        result => 'stderr_full_path'
    );

    $runner->setArtifactsNameBuilder($nameBuilderStub->object());

    my @input = buildPlayerAndGrabExecuteInput($runner, '--build-only', '--platform=standalone', '--testfilter=001');

    arrayContains(\@input, [$graphicsTestsRunnerMock, '--build-only', '--no-stdout']);

    my @artifacts = $runner->getArtifacts()->get();
    arrayContains(\@artifacts, ['path_to_built_players_root']);
}

Run_DefaultArguments_PlacesArtifactsPathToResultsCommandLine:
{
    my $runner = makeRunner();
    my @input = runAndGrabExecuteInput($runner);
    arrayContains(\@input, "--artifacts_path=" . $runner->getArtifactsPath());
}

Run_PlayerSavePath_PlaceItInResultingCommandLine:
{
    my @input = run('--player-save-path=players_root');
    arrayContains(\@input, "--player-save-path=players_root");
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

    my @input = listAndGrabExecuteInput($runner);
    arrayEndsWith(\@input, [$graphicsTestsRunnerMock, '--list', '--suite=graphics', '1>test_plan_full_file_name']);
}

Run_PrepareBuildTarget_InvokesPrepare:
{
    my $runner = makeRunner();
    $runner = new Test::MockObject::Extends($runner);

    my $target = new Test::MockObject();
    my $prepareCalled = 0;
    $target->mock(
        prepare => sub
        {
            $prepareCalled = 1;
        }
    );

    $target->mock(
        getModule => sub
        {
            return 'dummy';
        }
    );

    my $fileWatcher = new Test::MockModule('FileWatcher');
    $fileWatcher->mock(watch => sub { });
    $fileWatcher->mock(start => sub { });
    $fileWatcher->mock(stop => sub { });

    my $resolver = new Test::MockModule('BuildTargetResolver');
    $resolver->mock(resolveTarget => sub { return $target; });

    my @input = runAndGrabExecuteInput($runner, '--platform=psp2');
    is($prepareCalled, 1);
}

Run_MultipleDevices_AppearsCommaSeparatedInTheResultsCommandLine:
{
    my @input = run('--device=device1', '--device=device2');
    arrayContains(\@input, '--device=device1', '--device=device2');
}

Build_MultipleDevices_AppearsCommaSeparatedInTheResultsCommandLine:
{
    my $runner = makeRunner();
    my @input = buildPlayerAndGrabExecuteInput($runner, '--build-only', '--device=device1', '--device=device2');
    arrayContains(\@input, '--device=device1', '--device=device2');
}

List_MultipleDevices_AppearsCommaSeparatedInTheResultsCommandLine:
{
    my $runner = makeRunner();
    my $protocolModuleMock = new Test::MockModule('UnityTestProtocol::Utils');
    $protocolModuleMock->mock(getFirst => sub { return undef; });
    my @input = listAndGrabExecuteInput($runner, '--list', '--device=device1', '--device=device2');
    arrayContains(\@input, '--device=device1', '--device=device2');
}

Run_MultipleConfigs_AppearsCommaSeparatedInTheResultsCommandLine:
{
    my @input = run('--configuration=config1', '--configuration=config1');
    arrayContains(\@input, '--configuration=config1', '--configuration=config2');
}

Build_MultipleDevices_AppearsCommaSeparatedInTheResultsCommandLine:
{
    my $runner = makeRunner();
    my @input = buildPlayerAndGrabExecuteInput($runner, '--build-only', '--configuration=config1', '--configuration=config1');
    arrayContains(\@input, '--configuration=config1', '--configuration=config2');
}

List_MultipleDevices_AppearsCommaSeparatedInTheResultsCommandLine:
{
    my $runner = makeRunner();
    my $protocolModuleMock = new Test::MockModule('UnityTestProtocol::Utils');
    $protocolModuleMock->mock(getFirst => sub { return undef; });
    my @input = listAndGrabExecuteInput($runner, '--build-only', '--configuration=config1', '--configuration=config1');
    arrayContains(\@input, '--configuration=config1', '--configuration=config2');
}

Run_EditorLogsDir_PlaceItInResultCommandLine:
{
    my $editorLogPath = canonpath("/dev/void_and_darnkess");
    my @input = run("--editorlogsdir=$editorLogPath");
    arrayContains(\@input, "--editorlogsdir=$editorLogPath");
}

InvokesBaseClassWithProperConstructor:
{
    my $runnerOptionsLoaderMock = new Test::MockModule('Plugin');
    my %args;
    $runnerOptionsLoaderMock->mock(new => sub { (undef, %args) = @_; });
    my $runner = new SuiteRunners::Graphics();
    my $graphicsTestRunnerProj = canonpath("$root/Tests/Unity.GraphicsTestsRunner/Unity.GraphicsTestsRunner.csproj");
    is($args{optionsGenProj}, $unifiedTestRunnerCSharpProj);
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

Run_RunnerHasOneFlagOptionInRunnerOptionsFile_WillPassItToRunner:
{
    my $optionsReader = new Test::MockModule('RunnerOptions');

    my $option = new Option(
        names => ['flag-option'],
        valueType => Option->ValueTypeNone
    );

    $optionsReader->mock(readFromFile => sub { return ($option); });
    my $runner = makeRunner();

    my @input = buildPlayerAndGrabExecuteInput($runner, '--flag-option');
    arrayContains(\@input, '--flag-option');
}

List_RunnerHasOneFlagOptionInRunnerOptionsFile_WillPassItToRunner:
{
    my $optionsReader = new Test::MockModule('RunnerOptions');

    my $option = new Option(
        names => ['flag-option'],
        valueType => Option->ValueTypeNone
    );

    $optionsReader->mock(readFromFile => sub { return ($option); });

    my $protocolModuleMock = new Test::MockModule('UnityTestProtocol::Utils');
    $protocolModuleMock->mock(getFirst => sub { return undef; });

    my $runner = makeRunner();

    my @input = listAndGrabExecuteInput($runner, '--flag-option');

    arrayContains(\@input, '--flag-option');
}

Run_WatchesForProtocolMessagesInLog:
{
    my $runner = makeRunner();
    my %runParams;
    $runner->mock(
        runDotNetProgram => sub
        {
            (undef, %runParams) = @_;
        }
    );
    runAndGrabExecuteInput($runner);
    my $fileWatcher = $runParams{filesToWatch}[0];
    is($fileWatcher->{callback}, \&SuiteRunner::watchTestProtocolMessages);
    is($fileWatcher->{file}, $runner->getMachineLogPath());
}

Run_WatchesForProtocolMessagesInLog:
{

    my $fileUtilsMock = new Test::MockModule('FileUtils');
    $fileUtilsMock->mock(
        readJsonObj => sub
        {
            return { suites => [{ name => 'graphics' }] };
        }
    );

    my $runner = makeRunner();
    my $resultCollector = new ResultCollector();
    $runner->setResultConsumer($resultCollector);
    runAndGrabExecuteInput($runner);

    my @suites = $resultCollector->getAllSuiteResults();
    is($suites[0]->getName(), 'graphics');
}

if ($^O =~ /mswin32|mswin64|cygwin/i)
{
buildNativePluginDependenciesInvokesProperCommandLines:
    {

        my $environment = new Environments::Mono();
        my $msBuildMock = new Test::MockObject();
        my @calls;
        $msBuildMock->mock(
            build => sub
            {
                (undef, my %msBuildArgs) = @_;
                push(@calls, \%msBuildArgs);
            }
        );

        my $runner = makeRunner();
        $runner->mock(
            _getMSBuild => sub
            {
                return $msBuildMock;
            }
        );

        $runner->buildNativePluginDependencies();

        my $call1 = $calls[0];
        like($call1->{project}, qr/GfxPluginTests.vcxproj/);
        is($call1->{platform}, 'x64');
        is($call1->{config}, 'Release');
        like($call1->{logfile}, qr/GfxPluginTests.vcxproj-Release-x64-MSBuildLog.txt/);

        my $call2 = $calls[1];
        like($call2->{project}, qr/GfxPluginTests.vcxproj/);
        is($call2->{platform}, 'Win32');
        is($call2->{config}, 'Release');
        like($call2->{logfile}, qr/GfxPluginTests.vcxproj-Release-Win32-MSBuildLog.txt/);
    }
}

done_testing();

sub makeRunner
{
    my $runner = new SuiteRunners::Graphics();
    my $runContext = RunContext::new(new Options(options => $runner->{suiteOptions}));
    $runner->setRunContext($runContext);
    $runner = new Test::MockObject::Extends($runner);
    $runner->mock(_generateMSBuildScript => sub { return 'msbuild_file_name' });
    $runner->mock(_getAssemblyFullPath => sub { return $graphicsTestsRunnerMock; });
    $runner->mock(_getOutputBuildPath => sub { return 'build_path'; });

    if ($^O =~ /mswin32|mswin64|cygwin/i)
    {
        my $msBuildMock = new Test::MockObject();
        $msBuildMock->mock(build => sub { });
        $runner->mock(_getMSBuild => sub { return $msBuildMock; });
    }

    return $runner;
}

sub run
{
    my @args = @_;
    my $runner = makeRunner();
    my $nameBuilderStub = makeDefaultSuiteRunnerArtifactNameBuilderStub();
    $runner->setArtifactsNameBuilder($nameBuilderStub->object());
    return runAndGrabExecuteInput($runner, @args);
}
