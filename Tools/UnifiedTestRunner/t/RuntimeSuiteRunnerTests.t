use warnings;
use strict;

use Test::More;

use File::Spec::Functions qw (canonpath catfile);
use File::Path qw(remove_tree);

use Dirs;
use lib Dirs::external_root();
use Array::Utils qw (unique);
use File::Temp qw (tempdir);
use Test::Trap;
use SuiteRunners::Runtime;
use TestHelpers::Runners qw (runAndGrabExecuteInput listAndGrabExecuteInput makeDefaultSuiteRunnerArtifactNameBuilderStub runAndGrabBuildEngineInput);
use TestHelpers::Stub;
use TestHelpers::ArrayAssertions qw (arraysAreEqual arrayDoesNotContain arrayStartsWith arrayContains arrayEndsWith arrayDoesNotContain);
use TestHelpers::CSProjGen;
use Test::MockObject::Extends;
use Test::MockModule;
use Test::Deep 're';
use ResultCollector;
use JSON;
use Option;
use FileUtils;
use lib Dirs::external_root();
use Test::MockModule;

my ($mono, $root) = TestHelpers::Runners::getFileNames(artifacts => ['mono', 'root']);
my $allIn1RunnerProj = canonpath("$root/Tests/Unity.RuntimeTests.AllIn1Runner/Unity.RuntimeTests.AllIn1Runner.csproj");
my $unifiedTestRunnerCSharpProj = canonpath("$root/Tests/Unity.UnifiedTestRunner/Unity.UnifiedTestRunner.csproj");

my $fileUtilsMock = new Test::MockModule('FileUtils');
my @fileUtilsArgs;
$fileUtilsMock->mock(writeAllLines => sub { @fileUtilsArgs = @_; });
my $artifactsMapBuilderMock = new Test::MockModule('ArtifactsMapBuilder');
$artifactsMapBuilderMock->mock(fromNativeSuiteLog => sub { return new ArtifactsMap(); });

Run_DefaultArguments_PlacesStdOutAndStdErrAtTheEnd:
{
    my $runner = makeRunner();

    my $nameBuilderStub = makeDefaultSuiteRunnerArtifactNameBuilderStub();

    $nameBuilderStub->specifyExpectation(
        method => 'getFullAtrifactFileName',
        params => { 'parent-type' => 'AllInOneRunner', type => 'stdout', extension => 'txt' },
        result => 'stdout_full_path'
    );

    $nameBuilderStub->specifyExpectation(
        method => 'getFullAtrifactFileName',
        params => { 'parent-type' => 'AllInOneRunner', type => 'stderr', extension => 'txt' },
        result => 'stderr_full_path'
    );

    $runner->setArtifactsNameBuilder($nameBuilderStub->object());

    my @input = runAndGrabExecuteInput($runner);

    arrayEndsWith(\@input, ['1>stdout_full_path', '2>stderr_full_path']);
}

Run_DefaultArguments_PutsEdiorAsDefaultValueForPlatform:
{
    my $runner = makeRunner();

    my @input = runAndGrabExecuteInput($runner);

    arrayContains(\@input, '--platform=Editor');
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

Run_DefaultArguments_PutsRuntimeTestDllParameterInResultCommandLine:
{
    my $runner = makeRunner();

    $runner->mock(_getAssemblyFullPath => sub { return "RuntimeTests.dll"; });
    my @input = runAndGrabExecuteInput($runner);

    arrayContains(\@input, 'RuntimeTests.dll');
}

Run_DefaultArguments_DoesPutsABVInRunCommandLine:
{
    my $runner = makeRunner();

    my @input = runAndGrabExecuteInput($runner);

    arrayDoesNotContain(\@input, '--ABV');
}

Run_TestListIsSpecified_DoesNotBuildTheTestPlan:
{
    my $runner = makeRunner();

    my $buildTestPlanInvoked = 0;
    $runner->mock(
        buildTestPlan => sub
        {
            $buildTestPlanInvoked = 1;
        }
    );

    my @input = runAndGrabExecuteInput($runner, '--testlist=path_to_testlist');

    is($buildTestPlanInvoked, 0);
    arrayDoesNotContain(\@input, '--testlist=path_to_testlist');
    arrayContains(\@input, re(qr/--testlist=.*TestPlan.txt/));

    my @artifacts = $runner->getArtifacts()->get();
    arrayDoesNotContain(\@artifacts, 'path_to_testlist');
}

Run_TestFilterIsSpecified_DoesNotPutABVInResultCommandLine:
{
    my $runner = makeRunner();

    my @input = runAndGrabExecuteInput($runner, '--testfilter=abc');

    arrayDoesNotContain(\@input, '--ABV');
}

List_TestFilterIsSpecified_DoesNotPutABVInResultCommandLine:
{
    my $runner = makeRunner();

    my @input = listAndGrabExecuteInput($runner, '--testfilter=abc');

    arrayDoesNotContain(\@input, '--ABV');
}

Run_RunAllTests_DoesNotPutABVInResultCommandLine:
{
    my $runner = makeRunner();

    my @input = runAndGrabExecuteInput($runner, '--runalltests');

    arrayDoesNotContain(\@input, '--ABV');
}

List_DefaultArguments_PutsABVInResultCommandLine:
{
    my $runner = makeRunner();

    my @input = listAndGrabExecuteInput($runner, "--list");

    arrayContains(\@input, ["--ABV"]);
}

List_RunAllTests_DoesNotPutABVInResultCommandLine:
{
    my $runner = makeRunner();

    my @input = listAndGrabExecuteInput($runner, '--list', '--runalltests');

    arrayDoesNotContain(\@input, '--ABV');
}

Run_RegressionSuiteIsSpecified_DoesNotPutItIntoTheRunCommandLine:
{
    my $runner = makeRunner();

    my @input = runAndGrabExecuteInput($runner, '--regressionsuite');

    arrayDoesNotContain(\@input, '--regressionsuite');
}

List_RegressionSuiteIsSpecified_DoesNotPutABVInResultCommandLine:
{
    my $runner = makeRunner();

    my @input = listAndGrabExecuteInput($runner, '--list', '--regressionsuite');

    arrayDoesNotContain(\@input, '--ABV');
}

Run_DefaultArguments_PutsProjectPathToCommandLine:
{
    my $runner = makeRunner();

    my $nameBuilderStub = makeDefaultSuiteRunnerArtifactNameBuilderStub();
    $nameBuilderStub->specifyExpectation(
        method => 'getFullFolderPathForArtifactType',
        params => { type => 'UnityProject' },
        result => 'path_to_project'
    );

    $runner->setArtifactsNameBuilder($nameBuilderStub->object());

    my @input = runAndGrabExecuteInput($runner);

    my $projectPath = $runner->getSuiteArtifactsFolderName('Project');
    arrayContains(\@input, '--projectpath=path_to_project');
}

Run_DefaultArguments_NamesTestPlanAccordingToTheConvention:
{
    my $runner = makeRunner();

    my $nameBuilderStub = makeDefaultSuiteRunnerArtifactNameBuilderStub();

    $nameBuilderStub->specifyExpectation(
        method => 'getFullAtrifactFileName',
        params => { 'parent-type' => 'UnifiedTestRunner', type => 'TestPlan', extension => 'txt' },
        result => 'test_plan_full_file_name'
    );

    $runner->setArtifactsNameBuilder($nameBuilderStub->object());

    my @input = runAndGrabExecuteInput($runner);
    arrayContains(\@input, "--testlist=test_plan_full_file_name");
}

Run_DefaultArguments_NamesArtifactsFolderAccordingToTheConvention:
{
    my $runner = makeRunner();

    my $nameBuilderStub = makeDefaultSuiteRunnerArtifactNameBuilderStub();

    $runner->setArtifactsNameBuilder($nameBuilderStub->object());

    my @input = runAndGrabExecuteInput($runner);
    arrayContains(\@input, "--artifacts_path=" . $runner->getArtifactsPath());
}

Run_DefaultSuiteArguments_NamesStdOutArtifactAccordingToTheConvention:
{
    my $runner = makeRunner();
    my $nameBuilderStub = makeDefaultSuiteRunnerArtifactNameBuilderStub();

    $nameBuilderStub->specifyExpectation(
        method => 'getFullAtrifactFileName',
        params => { 'parent-type' => 'AllInOneRunner', type => 'stdout', extension => 'txt' },
        result => 'stdout_full_path'
    );

    $runner->setArtifactsNameBuilder($nameBuilderStub->object());

    runAndGrabExecuteInput($runner);

    my @artifacts = $runner->getArtifacts()->get();

    arrayContains(\@artifacts, 'stdout_full_path');
}

Run_DefaultSuiteArguments_NamesTestResultsXmlFileArtifactAccordingToTheConvention:
{
    my $runner = makeRunner();
    my $nameBuilderStub = makeDefaultSuiteRunnerArtifactNameBuilderStub();

    $nameBuilderStub->specifyExpectation(
        method => 'getFullAtrifactFileName',
        params => { 'parent-type' => 'AllInOneRunner', type => 'TestResults', extension => 'xml' },
        result => 'test_results_xml_full_file_name'
    );

    $runner->setArtifactsNameBuilder($nameBuilderStub->object());

    runAndGrabExecuteInput($runner);

    my @artifacts = $runner->getArtifacts()->get();

    arrayDoesNotContain(\@artifacts, 'test_results_xml_full_file_name');
}

Run_PlatformIsSpecified_PlaceItInResultCommandLine:
{
    my $runner = makeRunner();

    my @input = runAndGrabExecuteInput($runner, '--platform=standalone');

    arrayContains(\@input, '--platform=standalone');
}

Run_PlayerPlatformIsSpecified_PlacePlayerPathInResultComandLine:
{
    my $runner = makeRunner();

    my $nameBuilderStub = makeDefaultSuiteRunnerArtifactNameBuilderStub();

    $nameBuilderStub->specifyExpectation(
        method => 'getFullFolderPathForArtifactType',
        params => { type => 'PlayerBin' },
        result => 'player_bin_path'
    );

    $runner->setArtifactsNameBuilder($nameBuilderStub->object());

    my @input = runAndGrabExecuteInput($runner, '--platform=standalone');
    arrayContains(\@input, "--playerpath=player_bin_path");
}

Run_FilterIsSpecified_NotPlacedItInResultCommandLine:
{
    my $runner = makeRunner();

    my @input = runAndGrabExecuteInput($runner, '--testfilter=something');

    arrayDoesNotContain(\@input, "--testfilter=something");
}

List_DefaultArguments_PlacesExpectedArgumentsInsideResultCommandLine:
{
    my $runner = makeRunner();

    $runner->mock(_getAssemblyFullPath => sub { return "RuntimeTests.dll"; });

    my @input = listAndGrabExecuteInput($runner);

    arrayContains(\@input, ['--list', 'RuntimeTests.dll']);
}

List_RegressionSuiteIsSpecified_PlacesItInResultCommandLine:
{
    my @runnerArgs = ('--list', '--regressionsuite');
    my $runner = makeRunner();

    my @input = listAndGrabExecuteInput($runner, @runnerArgs);

    arrayContains(\@input, ['--regressionsuite']);
}

Run_ProjectPathIsSpecified_PlaceItInResultingCommandLine:
{
    my $runner = makeRunner();

    my @input = runAndGrabExecuteInput($runner, '--projectpath=/dev/null');

    my $devnull = canonpath("/dev/null");

    arrayContains(\@input, "--projectpath=$devnull");
}

Run_EditorLogPathIsSpecified_PlaceItInResultCommandLine:
{
    my $runner = makeRunner();
    my $editorLogPath = canonpath("/dev/void_and_darnkess");
    my @input = runAndGrabExecuteInput($runner, "--editorlogpath=$editorLogPath");
    arrayContains(\@input, "--editorlogpath=$editorLogPath");
}

Run_PlayerLogPathIsSpecified_PlaceItInResultCommandLine:
{
    my $runner = makeRunner();
    my $playerLogPath = canonpath("/dev/void_and_darnkess");
    my @input = runAndGrabExecuteInput($runner, "--playerlogpath=$playerLogPath", "--platform=standalone");
    arrayContains(\@input, "--playerlogpath=$playerLogPath");
}

Run_PlayerLogPathIsNotSpecified_DefaultValueIsGood:
{
    my $runner = makeRunner();
    my @input = runAndGrabExecuteInput($runner, '--platform=standalone');
    my $expectedPlayerPath = "--playerlogpath=" . $runner->_defaultPlayerLogName();
    arrayContains(\@input, $expectedPlayerPath);
}

Run_UnityArtifactsFolderSpecified_PlaceItInResultCommandLine:
{
    my $runner = makeRunner();

    my @input = runAndGrabExecuteInput($runner, '--unity-artifacts-folder=artifats_folder_path');

    my $expectedArtifactsPath = '--artifacts_path=artifats_folder_path';
    arrayContains(\@input, $expectedArtifactsPath);
}

Run_PlayerPathIsSpecified_PlaceItInResultCommandLine:
{
    my $runner = makeRunner();

    my @input = runAndGrabExecuteInput($runner, '--platform=standalone', '--playerpath=player_bin_path');

    arrayContains(\@input, "--playerpath=player_bin_path");
}

Run_PrepareBuildTarget_InvokesPrepare:
{
    my $runner = makeRunner();

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

    my @input = runAndGrabExecuteInput($runner, '--platform=android');
    is($prepareCalled, 1);
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
    my $runner = new SuiteRunners::Runtime();
    my $allIn1RunnerProj = canonpath("$root/Tests/Unity.RuntimeTests.AllIn1Runner/Unity.RuntimeTests.AllIn1Runner.csproj");

    is($args{optionsGenProj}, $unifiedTestRunnerCSharpProj);
}

Run_SmartTestSelectionOff_DoesNotAffectTestPlan:
{
    my $runner = makeRunner();
    my $testPlanReaderMock = new Test::MockModule('NUnitSuiteRunnerBase');

    my $fullTestPlan = new TestPlan();
    $testPlanReaderMock->mock(
        readTestPlanFromFile => sub
        {
            return $fullTestPlan;
        }
    );

    runAndGrabExecuteInput($runner);

    my $testPlan = $runner->getTestPlan();
    is_deeply($testPlan, $fullTestPlan);
}

Run_SmartTestSelectionOn_InvokesTestSelectorWithProperArguments:
{
    my $fullTestPlan = _createTestPlan('T1', 'T2');
    my %result = _executeWithSmartTestSelection(fullTestPlan => $fullTestPlan,);

    my $capabilities = Capabilities::getCapabilities();
    is_deeply(
        $result{queryTestsArgs},
        {

            #values belows comes from _executeWithSmartTestSelection
            uri => 'uri_value',
            revision => '1234',
            branch => 'trunk',
            configId => 'ConfId',
            submitter => 'submitterName',
            testPlan => $fullTestPlan,
            build => '5',
            capabilities => $capabilities
        }
    );
}

Run_SmartTestSelectionOn_AffectsTestPlan:
{
    my $fullTestPlan = _createTestPlan('T1', 'T2');
    my $filteredTestPlan = _createTestPlan('T2');

    my %result = _executeWithSmartTestSelection(
        fullTestPlan => $fullTestPlan,
        filteredTestPlan => $filteredTestPlan,
        skippedTests => ['T1']
    );

    is_deeply($result{testPlan}, $filteredTestPlan);
    arraysAreEqual(\@fileUtilsArgs, [re(qr/TestsExcludedBySmartSelect.txt/), 'T1']);
    arrayContains($result{artifacts}, [re(qr/TestsExcludedBySmartSelect.txt/)]);
}

Run_BadResponse_DoesNotAffectTestPlan:
{
    my $fullTestPlan = _createTestPlan('T1', 'T2');
    my $filteredTestPlan = undef;    # returned in case of error

    my %result = _executeWithSmartTestSelection(
        fullTestPlan => $fullTestPlan,
        filteredTestPlan => $filteredTestPlan
    );

    is_deeply($result{testPlan}, $fullTestPlan);
}

Run_SmartTestSelectionOn_AffectsTestPlanAndSendsMessage:
{
    my $fullTestPlan = _createTestPlan('T1', 'T2');
    my $filteredTestPlan = _createTestPlan('T2');
    my $messageSent = 0;

    my %result = _executeWithSmartTestSelection(
        fullTestPlan => $fullTestPlan,
        filteredTestPlan => $filteredTestPlan,
        skippedTests => ['T1'],
        messageSent => $messageSent
    );

    is($result{messageSent}, 1);
    is($result{totalTestsCount}, 2);
    is($result{skippedTestsCount}, 1);
}

Run_SmartTestSelectionOn_DoesNotAffectTestPlanAndSendsNoMessage:
{
    my $fullTestPlan = _createTestPlan('T1', 'T2');
    my $filteredTestPlan = undef;
    my $messageSent = 0;

    my %result = _executeWithSmartTestSelection(
        fullTestPlan => $fullTestPlan,
        filteredTestPlan => $filteredTestPlan,
        messageSent => $messageSent
    );

    is($result{messageSent}, 0);
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

Run_PlayerPathEditorPlatform_Croaks:
{
    my $runner = makeRunner();
    my @trap = trap
    {
        runAndGrabExecuteInput($runner, '--platform=editor', '--playerlogpath=blabla');
    };
    like($trap->die, qr/editor/);
}

Run_PlayerPathIsSpecifiedAndPlatformIsEditor_Croaks:
{
    my $runner = makeRunner();
    my @runnerArgs = ('--suite=runtime', '--platform=editor', '--playerpath=/tmp/123');

    my @trap = trap
    {
        runAndGrabExecuteInput($runner, @runnerArgs);
    };
    like($trap->die, qr/playerpath/);
}

Run_ArtifactsMapIsProduced_RegisterItAsArtifacts:
{
    my $runner = makeRunner();
    my $artifactsMapBuilderMock = new Test::MockModule('ArtifactsMapBuilder');
    $artifactsMapBuilderMock->mock(fromRuntimeSuiteLog => sub { return new ArtifactsMap(); });

    $artifactsMapBuilderMock->mock(
        fromRuntimeSuiteLog => sub
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

done_testing();

sub makeRunner
{
    my $runner = new SuiteRunners::Runtime();
    $runner = new Test::MockObject::Extends($runner);
    my $runContext = RunContext::new(new Options(options => $runner->{suiteOptions}));
    $runner->setRunContext($runContext);
    $runner->mock(_getOutputBuildPath => sub { return 'build_path'; });
    $runner->mock(_getAssemblyFullPath => sub { return 'assembly_full_path'; });
    $runner->mock(_generateMSBuildScript => sub { return 'msbuild_file_name' });
    $runner->mock(_getMemoryLeaks => sub { });
    $runner->mock(_readTestPlanFromMachineLog => sub { return new TestPlan() });
    $runner->mock(processResults => sub { });
    return $runner;
}

sub _createTestPlan
{
    my @tests = @_;
    my $result = new TestPlan();
    foreach my $t (@tests)
    {
        my $testItem = new TestPlanItem(name => $t, testCaseCount => 1);
        $result->registerTest($testItem);
    }
    return $result;
}

sub _executeWithSmartTestSelection
{
    my %args = @_;
    my $fullTestPlan = $args{fullTestPlan};
    my $filteredTestPlan = $args{filteredTestPlan};
    my $skippedTests = $args{skippedTests};
    my $messageSent = $args{messageSent};
    my $totalTestsCount = 0;
    my $skippedTestsCount = 0;

    my $messages = new Test::MockModule('UnityTestProtocol::Messages');
    $messages->mock(
        makeSmartSelectMessage => sub
        {
            my ($smartSelectData) = @_;
            $totalTestsCount = $smartSelectData->{total};
            $skippedTestsCount = $smartSelectData->{skipped};
        }
    );

    my $eventLoggerMock = new Test::MockModule('RemoteEventLogger');
    $eventLoggerMock->mock(
        log => sub
        {
            $messageSent = 1;
        }
    );

    my $testPlanReaderMock = new Test::MockModule('RuntimeBase');
    $testPlanReaderMock->mock(
        readTestPlanFromFile => sub
        {
            return $fullTestPlan;
        }
    );

    my $smartTestSelectorMock = new Test::MockModule('SmartTestsSelector');
    my %queryTestsArgs;
    $smartTestSelectorMock->mock(
        queryTests => sub
        {
            %queryTestsArgs = @_;
            return ($filteredTestPlan, $skippedTests);
        }
    );

    my $repositoryInfo = new Test::MockModule('RepositoryInfo');
    $repositoryInfo->mock(
        get => sub
        {
            return (branch => 'trunk', revision => '1234');
        }
    );

    my $runner = makeRunner();
    $runner->mock(_readTestPlanFromMachineLog => sub { return $fullTestPlan; });
    runAndGrabExecuteInput($runner, '--smart-select', '--config-id=ConfId', '--submitter=submitterName', '--smart-tests-selector-uri=uri_value',
        '--build-number=5');

    my %result;
    $result{testPlan} = $runner->getTestPlan();
    $result{queryTestsArgs} = \%queryTestsArgs;
    my @artifacts = $runner->getArtifacts()->get();
    $result{artifacts} = \@artifacts;
    $result{messageSent} = $messageSent;
    $result{totalTestsCount} = $totalTestsCount;
    $result{skippedTestsCount} = $skippedTestsCount;
    return %result;
}
