use warnings;
use strict;

use Dirs;
use Test::More;
use TestHelpers::Runners;
use File::Temp qw (tempdir);
use Array::Utils qw (unique);
use File::Spec::Functions qw (catfile);
use TestHelpers::Stub;
use File::Spec::Functions qw (canonpath);
use File::Path qw(remove_tree);
use TestHelpers::Runners
    qw (runAndGrabExecuteInput makeRunnerMock listAndGrabExecuteInput runAndGrabBuildEngineInput makeDefaultSuiteRunnerArtifactNameBuilderStub);
use TestHelpers::ArrayAssertions qw (arrayStartsWith arrayDoesNotContain arrayContains arrayEndsWith arraysAreEqual);
use TestHelpers::CSProjGen;
use lib Dirs::external_root();
use Test::MockObject::Extends;
use Test::MockModule;
use Test::Deep qw(cmp_deeply re all array_each);
use FileUtils;
use TestHelpers::TestPlan;
use Switch;
use Test::Trap;

BEGIN
{
    use_ok('SuiteRunners::Integration');
}

my $solutionPath = generateSolution();

my $fileUtilsMock = new Test::MockModule('FileUtils');
my @fileUtilsArgs;
$fileUtilsMock->mock(writeAllLines => sub { @fileUtilsArgs = @_; });
$fileUtilsMock->mock(readAllLines => sub { });

my $testProtocolUtilsMock = new Test::MockModule('UnityTestProtocol::Utils');
$testProtocolUtilsMock->mock(getFirst => sub { });

my ($mono, $root, $lister) = TestHelpers::Runners::getFileNames(artifacts => ['mono', 'root', 'lister']);

Run_DefaultArguments_ProducesCorrectCommandLineAndArtifacts:
{
    my $runner = makeRunner();

    my $nameBuilderStub = makeDefaultSuiteRunnerArtifactNameBuilderStub();
    $nameBuilderStub->specifyExpectation(
        method => 'getFullFolderPathForArtifactType',
        params => { type => 'Artifacts' },
        result => 'path_to_artifacts'
    );
    $runner->setArtifactsNameBuilder($nameBuilderStub->object());

    my @input = runAndGrabExecuteInput($runner);
    my $artifactsPath = $runner->getArtifactsPath();

    arraysAreEqual(
        \@input,
        [
            $mono, '--debug', re(qr/Unity.UnifiedTestRunner[.]exe/),
            "--suite=integration",
            "--artifacts_path=$artifactsPath",
            "--testlist=" . canonpath(catfile($artifactsPath, "TestList.txt")),
            re('Unity.IntegrationTests.dll'),
            re('Unity.PureCSharpTests.dll'),
        ]
    );
}

List_Default_ProducesCorrectCommandLine:
{
    my @runnerArgs = ('--list');
    my $runner = makeRunner();

    my @input = listAndGrabExecuteInput($runner, '--list', '--testtarget=TestDummy');

    my $artifactsPath = $runner->getArtifactsPath();
    arrayStartsWith(
        \@input,
        [
            $mono, '--debug',
            re(qr/Unity.UnifiedTestRunner[.]exe/), '--suite=integration',
            '--list', "--artifacts_path=$artifactsPath",
            '--no-stdout', '--include=DummyInclude1',
            '--include=DummyInclude2', '--exclude=DummyExclude1',
            '--exclude=DummyExclude2', re('Unity.IntegrationTests.dll'),
            re('Unity.PureCSharpTests.dll'),
        ]
    );
}

Run_UnityArtifacts_SetsEnvironmentVariable:
{
    is(getEnvVarValueDuringTheRun('UNITY_UNIFIED_TEST_RUNNER_UNITY_ARTIFACTS_FOLDER', '--unity-artifacts-folder=/dev/null'), canonpath('/dev/null'));
}

Run_ForceUsbConnectionIsDefined_SetsEnvironmentVariable:
{
    is(getEnvVarValueDuringTheRun('UNITY_FORCE_ANDROID_USB_CONNECTION', '--forceusbconnection'), 1);
}

Run_ForceUsbConnectionIsNotDefined_DoesNotSetEnvironmentVariable:
{
    is(getEnvVarValueDuringTheRun('UNITY_FORCE_ANDROID_USB_CONNECTION'), undef);
}

Run_AssetPipelineV2IsDefined_SetsEnvironmentVariable:
{
    is(getEnvVarValueDuringTheRun('UNITY_USE_ASSET_PIPELINE_V2', '--assetpipelinev2'), 1);
}

Run_AssetPipelineV2IsDefined_SetsEnvironmentVariable:
{
    is(getEnvVarValueDuringTheRun('UNITY_USE_ASSET_PIPELINE_V2'), undef);
}

Run_SeveralProjectsAreSpecified_PutsPathToTheirDllsToResultCommandLine:
{
    my @input = run("--testproject=Unity.IntegrationTests.gen.csproj", "--testprojects=Unity.PureCSharpTests.csproj");

    arrayEndsWith(\@input, [re(qr/--testlist=.*/), re(qr/Unity.IntegrationTests.dll/), re(qr/Unity.PureCSharpTests.dll/)]);
}

Run_PartialMatchForTestProjects_ReturnsOnlyMatchingProjects:
{
    my @input = run("--testprojects=IntegrationTests");

    @input = grep { /[.]dll$/ } @input;
    my $dllnameContainsIntegrationTestsString = all(re(qr/IntegrationTests.*dll/i));
    cmp_deeply(\@input, array_each($dllnameContainsIntegrationTestsString));
}

Run_AllProjectsAreSpecified_PutsPathToTheirDllsToResultCommandLine:
{
    my $runner = makeRunner();

    my @input = run("--testprojects=All");

    my @allProjects = $runner->_getAllCSProjects();
    arrayContains(\@input, [re('Unity.IntegrationTests.dll'), re('Unity.PureCSharpTests.dll')]);
}

Run_DefaultArguments_WatchingStdOutAndStdErr:
{
    my $runner = makeRunner();

    runAndGrabExecuteInput($runner);

    my $system = $runner->getEnvironment()->getSystemCall();
    my @filesToWatch = $system->getFilesToWatch();
    my @files = unique(map { $_->{file} } @filesToWatch);
    arrayContains(\@files, re('TestRunnerLog.txt'));
}

Run_SmartTestSelectionOff_DoesNotAffectTestPlan:
{
    my $runner = makeRunner();
    my $fullTestPlan = new TestPlan();
    my $testPlanReaderMock = new Test::MockModule('TestPlan');
    $testPlanReaderMock->mock(
        getTestPlanFromTestProtocolMessage => sub
        {
            return $fullTestPlan;
        }
    );

    $testPlanReaderMock->mock(
        getTestPlanFromTestProtocolMessage => sub
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
    my %result = _executeWithSmartTestSelection(fullTestPlan => $fullTestPlan);

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

    my @fileUtilsArgs;
    $fileUtilsMock->mock(writeAllLines => sub { push(@fileUtilsArgs, @_); });

    my %result = _executeWithSmartTestSelection(
        fullTestPlan => $fullTestPlan,
        filteredTestPlan => $filteredTestPlan,
        skippedTests => ['T1']
    );

    is_deeply($result{testPlan}, $filteredTestPlan);

    arraysAreEqual(\@fileUtilsArgs, [re('TestsExcludedBySmartSelect.txt'), 'T1', re('TestList.txt'), 'T2']);
    arrayContains($result{artifacts}, re('TestsExcludedBySmartSelect.txt'));
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

Run_PrepareBuildTargets_InvokesPrepareOnTargets:
{
    my $runner = makeRunner();
    $runner->mock(
        getRuntimePlatforms => sub
        {
            return ('Platform1');
        }
    );
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

    runAndGrabExecuteInput($runner, '--testprojects=All');

    is($prepareCalled, 1);
}

Run_IsolationModeIsEnabledByDefault:
{
    my $runner = makeRunner();
    is($runner->getOptionValue('build-isolation'), 'enabled');
}

Run_DefaultBuildModeIsMinimal:
{
    my $runner = makeRunner();
    is($runner->getOptionValue('build-mode'), 'minimal');
}

Run_BuildAutomationDependencies_InvokesPrepareOnTargets:
{
    my $runner = makeRunner();
    $runner = new Test::MockObject::Extends($runner);

    $runner->mock(
        getRuntimePlatforms => sub
        {
            return ('Platform1');
        }
    );

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

    my %buildEngineInput = runAndGrabBuildEngineInput($runner, '--testprojects=All');

    is($invokedWithPlatforms, 'Platform1');
    is($buildEngineInput{project}, 'Platform1.Player.csproj');
    is($buildEngineInput{rebuild}, 0);
}

Run_CanLoadTestsFromPlainTestList:
{
    my $testPlanMock = new Test::MockModule('TestPlan');
    $testPlanMock->mock(
        loadFromPlainTestListFile => sub
        {
            return _createTestPlan('T1', 'T2');
        }
    );
    my $runner = makeRunner();
    $runner->mock(writeTestPlan => sub { });
    my @input = runAndGrabExecuteInput($runner, '--testlist=path_to_test_list');
    my $testPlan = $runner->getTestPlan();
    my @tests = map { $_->getName() } $testPlan->getAllTestItems();
    is_deeply(\@tests, ['T1', 'T2']);
}

Run_CanLoadFromPartitionDataFile:
{
    my $testPlanMock = new Test::MockModule('TestPlan');
    $testPlanMock->mock(
        loadFromPartitionsJsonFile => sub
        {
            my ($fileName, $partitionIndex) = @_;
            if ($fileName eq '/tmp/partition/data' and $partitionIndex eq 1)
            {
                return _createTestPlan('T1', 'T2');
            }
        }
    );
    my $runner = makeRunner();
    $runner->mock(writeTestPlan => sub { });

    my @input = runAndGrabExecuteInput($runner, '--partition-source=/tmp/partition/data', '--partition-index=1');

    my $testPlan = $runner->getTestPlan();
    my @tests = map { $_->getName() } $testPlan->getAllTestItems();
    is_deeply(\@tests, ['T1', 'T2']);
}

Run_CanLoadFromFromRemoteWebService:
{
    my $response = <<END_JSON;
{
  "FilteredTests": [],
  "Partitions": [
    [
      {
        "Name": "test1"
      },
      {
        "Name": "test3"
      }
    ],
    [
      {
        "Name": "test2"
      },
      {
        "Name": "test4"
      }
    ]
  ]
}
END_JSON
    my $parallelTestSelector = new Test::MockModule('ParallelTestsSelector');
    my %queryArgs;
    $parallelTestSelector->mock(
        queryPartitionsById => sub
        {
            %queryArgs = @_;
            return $response;
        }
    );

    my $runner = makeRunner();
    my @input = runAndGrabExecuteInput($runner, '--partitions-id=42', '--partition-index=1');

    my $testPlan = $runner->getTestPlan();
    my @tests = map { $_->getName() } $testPlan->getAllTestItems();
    is_deeply(\@tests, ['test2', 'test4']);
    is($queryArgs{uri}, $runner->getOptionValue('parallel-tests-selector-uri'));
    is($queryArgs{id}, $runner->getOptionValue('partitions-id'));
}

Run_CanLoadFromFromRemoteWebService:
{
    my $response = <<END_JSON;
{
  "FilteredTests": [],
  "Partitions": [
    [
      {
        "Name": "test1"
      },
      {
        "Name": "test3"
      }
    ],
    [
      {
        "Name": "test2"
      },
      {
        "Name": "test4"
      }
    ]
  ]
}
END_JSON
    my $parallelTestSelector = new Test::MockModule('ParallelTestsSelector');
    my %queryArgs;
    $parallelTestSelector->mock(
        queryPartitionsById => sub
        {
            %queryArgs = @_;
            return $response;
        }
    );

    my $runner = makeRunner();
    my @input = runAndGrabExecuteInput($runner, '--partitions-id=42', '--partition-index=1', '--testfilter=t.*2');
    my $testPlan = $runner->getTestPlan();
    my @tests = map { $_->getName() } $testPlan->getAllTestItems();
    is_deeply(\@tests, ['test2']);
}

queryPartitions_BasicScenario:
{
    my $parallelTestSelector = new Test::MockModule('ParallelTestsSelector');
    my $repositoryInfo = new Test::MockModule('RepositoryInfo');
    $repositoryInfo->mock(
        get => sub
        {
            return (branch => 'trunk', revision => '1234');
        }
    );

    my $response = {};
    my %queryArgs;
    $parallelTestSelector->mock(
        queryPartitions => sub
        {
            %queryArgs = @_;
            return {};
        }
    );

    my @args = ('--config-id=config-id-value', '--submitter=submitter-value', '--partition-count=4', '--smart-select');

    my $runner = makeRunnerMock(makeRunner(), @args);
    my $testPlan = TestHelpers::TestPlan::createTestPlanFromTestObjects('T1', 'T2');
    $runner->mock(
        getTestPlan => sub
        {
            return $testPlan;
        }
    );
    $runner->queryPartitions();

    is($queryArgs{uri}, $runner->getOptionValue('parallel-tests-selector-uri'));
    is($queryArgs{branch}, 'trunk');
    is($queryArgs{revision}, '1234');
    is($queryArgs{configId}, 'config-id-value');
    is($queryArgs{submitter}, 'submitter-value');
    is($queryArgs{partitionCount}, 4);
    is($queryArgs{filterUsingSmartSelect}, 1);
    is($queryArgs{timeout}, $runner->getOptionValue('query-partitions-timeout'));
    is($queryArgs{retries}, $runner->getOptionValue('query-partitions-retries'));
    is_deeply($queryArgs{testPlan}, $testPlan);
}

queryPartitions_PartitionCountIsNotSpecified:
{
    my $parallelTestSelector = new Test::MockModule('ParallelTestsSelector');
    my $repositoryInfo = new Test::MockModule('RepositoryInfo');
    $repositoryInfo->mock(
        get => sub
        {
            return (branch => 'trunk', revision => '1234');
        }
    );

    my $response = {};
    my %queryArgs;
    $parallelTestSelector->mock(
        queryPartitions => sub
        {
            %queryArgs = @_;
            return {};
        }
    );

    my @args = ('--config-id=config-id-value', '--submitter=submitter-value', '--smart-select');

    my $runner = makeRunnerMock(makeRunner(), @args);
    my $testPlan = TestHelpers::TestPlan::createTestPlanFromTestObjects('T1', 'T2');
    $runner->mock(
        getTestPlan => sub
        {
            return $testPlan;
        }
    );

    $runner->queryPartitions();

    is($queryArgs{partitionCount}, undef);
}

done_testing();

sub makeRunner
{
    my (@allProjects) = @_;
    my $runner = new SuiteRunners::Integration();
    $runner = new Test::MockObject::Extends($runner);
    my $runContext = RunContext::new(new Options(options => $runner->{suiteOptions}));
    $runner->setRunContext($runContext);
    $runner->mock(_getOutputBuildPath => sub { return 'build_path'; });
    $runner->mock(_generateMSBuildScript => sub { return 'msbuild_file_name' });
    $runner->mock(_getSlnFileName => sub { return $solutionPath; });
    $runner->mock(buildNUnitTestProtocolAddin => sub { });

    if (scalar(@allProjects) == 0)
    {
        @allProjects = ("Unity.IntegrationTests.gen.csproj", "Unity.PureCSharpTests.csproj");
    }

    $runner->mock(
        _getAllCSProjects => sub
        {
            return @allProjects;
        }
    );
    $runner->mock(writeSuiteData => sub { });
    $runner->mock(_reportPerformanceData => sub { });
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

    my $testPlanReaderMock = new Test::MockModule('TestPlan');
    $testPlanReaderMock->mock(
        getTestPlanFromTestProtocolMessage => sub
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

sub generateSolution
{
    my $solutionDir = tempdir();

    my $solutionContent =
        qq (Project("{FAE04EC0-301F-11D3-BF4B-00C04F79EFBC}") = "Unity.PureCSharpTests", "$solutionDir/Unity.PureCSharpTests.csproj", "{00591A5F-21D5-4B96-A93E-A29FF452A8FD}"
EndProject
Project("{FAE04EC0-301F-11D3-BF4B-00C04F79EFBC}") = "Unity.IntegrationTests.gen", "$solutionDir/Unity.IntegrationTests.gen.csproj", "{D9BCBF7B-886F-4C6B-FAD7-C1F35973A3C4}"
EndProject
);
    my $solutionFile = canonpath("$solutionDir/Unity.CSharpProjects.gen.sln");
    FileUtils::writeAllLines("$solutionDir/Unity.CSharpProjects.gen.sln", $solutionContent);

    my $pureCSProj = TestHelpers::CSProjGen::getText("Unity.PureCSharpTests");
    FileUtils::writeAllLines("$solutionDir/Unity.PureCSharpTests.csproj", $pureCSProj);

    my $intProj = TestHelpers::CSProjGen::getText("Unity.IntegrationTests");
    FileUtils::writeAllLines("$solutionDir/Unity.IntegrationTests.gen.csproj", $intProj);

    return $solutionFile;
}

sub getEnvVarValueDuringTheRun
{
    my ($envVarName, @args) = @_;
    my $runner = makeRunner();
    $runner = TestHelpers::Runners::makeRunnerMock($runner, @args);
    my $envStateDuringRun;
    $runner->mock(
        'doRun' => sub
        {
            my ($this) = @_;
            $envStateDuringRun = $ENV{$envVarName};
            $this->{runnerExitCode} = 0;
        },
    );

    delete $ENV{$envVarName};
    $runner->run();

    return $envStateDuringRun;
}

sub run
{
    my @args = @_;
    my $runner = makeRunner();
    my $nameBuilderStub = makeDefaultSuiteRunnerArtifactNameBuilderStub();
    $runner->setArtifactsNameBuilder($nameBuilderStub->object());
    return runAndGrabExecuteInput($runner, @args);
}

END
{
    remove_tree($solutionPath);
}
