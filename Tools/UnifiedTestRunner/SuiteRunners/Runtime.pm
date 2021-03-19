package SuiteRunners::Runtime;

use parent RuntimeBase;

use warnings;
use strict;

use Dirs;
use LineTail;
use threads::shared;
use File::Spec::Functions qw (canonpath catfile);
use Carp qw (croak);
use ArtifactsFolderDeleterGuard;
use FolderDeleter;
use SuiteRunnerArtifactsNameBuilder;
use TestPlan;
use UnityTestProtocol::Utils;
use RunnerOptions;
use Option;
use MSBuildProjectGenerator;

my $root = Dirs::getRoot();

my $runtimeTestRunnerProj = canonpath("$root/Tests/Unity.RuntimeTestsRunner/Unity.RuntimeTestsRunner.csproj");
my $unifiedTestRunnerCSharpProj = canonpath("$root/Tests/Unity.UnifiedTestRunner/Unity.UnifiedTestRunner.csproj");

my $suiteOptions = [
    {
        prototypes => ['regressionsuite:s', 'testfilter:s']
    },
    ['runalltests', 'By default tests are filtered to ABV (build verification) test set. Use this parameter to run all Runtime tests.'],

    #TODO overridie build  default value until it become, global default
    ['build-mode:s', 'Build mode', { hidden => 1, defaultValue => 'minimal' }],
    ['build-isolation:s', undef, { hidden => 1, defaultValue => 'enabled' }],
    ['smart-select', 'Enables smart tests selection based on hoader statistics data', { defaultValue => 0, hidden => 1 }],
    [
        'smart-tests-selector-uri:s',
        'Smart tests selector web service uri',
        { defaultValue => 'http://api.hoarder.qa.hq.unity3d.com/v1/SmartTestsSelector', hidden => 1 }
    ],
    ['unity-artifacts-folder:s', 'Path to store various unity artifacts such as editor build log, player log, etc.', { hidden => 1 }],
    [
        'autodoc-file:s',
        'Path to autodocs option file',
        {
            hidden => 1,
            defaultValue => "$root/Tests/Docs/docs/TestFrameworks/RuntimeTests/UnifiedTestRunner.Options.generated.md"
        }
    ],
];

sub new
{
    my ($pkg) = @_;
    return $pkg->SUPER::new(
        suiteOptions => $suiteOptions,
        optionsGenProj => $unifiedTestRunnerCSharpProj,
        testRunnerLibProj => $runtimeTestRunnerProj,
    );
}

sub usage
{
    my ($this) = @_;
    return (
        description => 'runtime tests',
        url => 'http://internaldocs.hq.unity3d.com/automation/TestFrameworks/RuntimeTests/About/index.html',
        examples => [
            { description => 'Run regressions runtime tests', command => '--regressionsuite' },
            { description => 'Run ABV runtime tests', command => '--ABV' },
            { description => 'Run tests from \'PhysicsTests\' area', command => '--area=PhysicsTests' }
        ],
    );
}

sub getName
{
    my ($this) = @_;
    return 'runtime';
}

sub getPlatform
{
    my ($this) = @_;
    return $this->getOptionValue('platform', 'editor');
}

sub buildDependencies
{
    my ($this) = @_;
    if ($this->{dependencies_built})
    {
        return;
    }
    $this->cleanCSharpProjectsSln();
    my $runtimeTestsProj = $this->_getRuntimeTestsProjPath();
    $this->buildCSDependencies($unifiedTestRunnerCSharpProj, $runtimeTestRunnerProj, $runtimeTestsProj);
    $this->{dependencies_built} = 1;
}

sub doRun
{
    my ($this) = @_;
    $this->prepareBuildTarget($this->getPlatform());
    my @args = $this->prepareArgsForRun();
    my $stdOut = $this->stdOutFileName();
    my $stdErr = $this->stdErrFileName();

    my $unifiedTestRunner = $this->getPathToTestRunner();

    $this->{runnerExitCode} = $this->runDotNetProgram(
        program => $unifiedTestRunner,
        programArgs => \@args,
        filesToWatch => [
            { file => $stdOut, callback => \&SuiteRunner::watchTestProtocolMessages, callbackArg => $this },
            { file => $stdOut, callback => \&RuntimeBase::printProgressMessage, callbackArg => $this },
            { file => $stdOut, callback => \&Plugin::printLogMessage, callbackArg => $this },
            { file => $stdErr, callback => \&Plugin::printLogErrorMessage, callbackArg => $this }
        ]
    );
}

sub prepareArgsForRun
{
    my ($this) = @_;
    my @args = ();

    push(@args, "--suite=" . $this->getName());

    my $platform = $this->getPlatform();
    push(@args, "--platform=$platform");

    my $runtimeTestsProj = $this->_getRuntimeTestsProjPath();
    my $testAssembly = $this->_getAssemblyFullPath($runtimeTestsProj);

    #push(@args, "--assemblywithtests=$testAssembly");

    my $testPlanFileName = $this->testPlanFileName();
    push(@args, "--testlist=$testPlanFileName");

    if ($this->projectPath())
    {
        if (-d $this->projectPath())
        {
            croak($this->projectPath . " already exists");
        }
        push(@args, "--projectpath=" . $this->projectPath());
    }

    my $editorLogPath = $this->editorLogPath();
    if (defined($editorLogPath))
    {
        push(@args, "--editorlogpath=$editorLogPath");
    }

    my $playerLogPath = $this->getOptionValue('playerlogpath');
    my $isEditorPatform = $this->isEditor();
    if ($isEditorPatform and defined($playerLogPath))
    {
        croak("playerlogpath is irrelevant when for editor platform");
    }

    if (not $isEditorPatform)
    {
        if (not defined($playerLogPath))
        {
            $playerLogPath = $this->_defaultPlayerLogName();
        }
        push(@args, "--playerlogpath=$playerLogPath");
    }

    my $playerPath = $this->getOptionValue('playerpath');
    if (defined($playerPath) and lc($platform) eq 'editor')
    {
        croak("playerpath is irrelevant when for editor platform");
    }

    if (not $isEditorPatform)
    {
        $playerPath = $this->playerBinPath();
        push(@args, "--playerpath=$playerPath");
    }

    my $artifactsFolder = $this->artifactsFolder();
    push(@args, "--artifacts_path=$artifactsFolder");

    $this->_addRunnerOptions(\@args);

    my $stdOut = $this->stdOutFileName();
    push(@args, "1>$stdOut");

    my $stdErr = $this->stdErrFileName();
    push(@args, "2>$stdErr");

    return @args;
}

sub playerBinPath
{
    my ($this) = @_;
    my $result = $this->getOptionValue('playerpath');
    if (not $result)
    {
        $result = RuntimeBase::defaultPlayerPath($this);
    }
    return $result;
}

sub onMessage
{
    my ($this, $msg) = @_;
    if ($msg->{type} eq 'TestFinish')
    {
        $this->processTestFinishMessage($msg);
    }
}

sub prepareArgsForListTests
{
    my ($this) = @_;
    my @args = ();

    push(@args, "--suite=" . $this->getName());
    push(@args, "--list");
    push(@args, "--no-stdout");
    push(@args, "--artifacts_path=" . $this->getArtifactsPath());

    my $platform = $this->getPlatform();
    push(@args, "--platform=$platform");

    my $filter = $this->getOptionValue('testfilter');
    if (defined($filter))
    {
        push(@args, "--testfilter=$filter");
    }

    my $runtimeTestsProj = $this->_getRuntimeTestsProjPath();
    my $testAssembly = $this->_getAssemblyFullPath($runtimeTestsProj, 1);

    #push(@args, "--assemblywithtests=$testAssembly");

    if ($this->getOptionValue('regressionsuite'))
    {
        push(@args, "--regressionsuite");
    }    # TODO: Pass --ABV parameter from Katana and remove this
    else
    {
        my $testfilter = $this->getOptionValue('testfilter');
        if (not defined $this->getOptionValue('runalltests') and not defined($testfilter))
        {
            push(@args, "--ABV");
        }
    }

    $this->_addRunnerOptions(\@args);
    return @args;
}

sub getTestPlan
{
    my ($this) = @_;

    if (defined($this->{testPlan}))
    {
        return $this->{testPlan};
    }

    my $testlist = $this->getOptionValue('testlist');
    if ($testlist)
    {

        Logger::minimal("Reading test plan from $testlist");
        $this->{testPlan} = $this->readTestPlanFromFile($testlist);
    }
    else
    {
        $this->{testPlan} = $this->buildTestPlan();
    }

    # Can be overriden in subclasses:
    $this->{testPlan} = $this->adjustTestPlan($this->{testPlan});
    return $this->{testPlan};
}

sub buildTestPlan
{
    my ($this) = @_;
    my @args = $this->prepareArgsForListTests();
    my $machineLog = $this->getMachineLogPath();
    my $exitCode = $this->runDotNetProgram(
        program => $this->getPathToTestRunner(),
        programArgs => \@args
    );

    if ($exitCode != 0)
    {
        croak($this->getPathToTestRunner() . " exited with $exitCode");
    }

    return $this->_readTestPlanFromMachineLog($machineLog);
}

sub _readTestPlanFromMachineLog
{
    my (undef, $machineLog) = @_;
    return TestPlan::readTestPlanFromMachineLog($machineLog);
}

sub getHumanLogPath
{
    my ($this) = @_;
    return catfile($this->getArtifactsPath(), "TestRunnerLog.txt");
}

sub getMachineLogPath
{
    my ($this) = @_;
    return catfile($this->getArtifactsPath(), "TestRunnerLog.json");
}

sub adjustTestPlan
{
    my ($this, $testPlan) = @_;

    return $this->applySmartSelect($testPlan);
}

sub afterRun
{
    my ($this) = @_;
    $this->SUPER::afterRun();
    my $artifactsMap = $this->getArtifactsMap();
    if (not $artifactsMap)
    {
        return;
    }
    foreach my $art ($artifactsMap->getArtifacts())
    {
        $this->registerArtifacts($art);
    }

    if ($this->{runnerExitCode} != 0)
    {
        $this->registerArtifact($this->projectPath());
        if (not $this->isEditor())
        {
            $this->registerArtifact($this->playerBinPath());
        }
    }
}

sub getArtifactsMap
{
    my ($this) = @_;
    my $stdOut = $this->stdOutFileName();
    return ArtifactsMapBuilder::fromRuntimeSuiteLog($stdOut, $this->getArtifactsPath());
}

sub getPathToTestRunner
{
    my ($this) = @_;
    return $this->_getAssemblyFullPath($unifiedTestRunnerCSharpProj);
}

sub editorLogPath
{
    my ($this) = @_;

    my $logFilePath = $this->getOptionValue('editorlogpath');
    if (defined($logFilePath))
    {
        return canonpath($logFilePath);
    }
    return catfile($this->getSuiteRunArtifactsRoot(), 'Editor.log');
}

sub processResults
{
    my ($this) = @_;
    my $buildOnly = $this->getOptionValue('--build-only');
    if ($buildOnly)
    {
        return;    # no results expected
    }

    my $jsonReportPath = catfile($this->getArtifactsPath(), 'TestReportData.json');
    my $testReport = FileUtils::readJsonObj($jsonReportPath);
    my $suite = $testReport->{suites}->[0];
    my $suiteResult = SuiteResult::fromJson($suite);
    $this->writeSuiteData($suiteResult);
    $this->sendResultToConsumer($suiteResult);
}

sub _defaultPlayerLogName
{
    my ($this) = @_;
    return catfile($this->getSuiteRunArtifactsRoot(), 'Player.' . $this->getPlatform() . '.log');
}

sub _getRuntimeTestsProjPath
{
    my ($this) = @_;
    return canonpath("$root/Tests/Unity.RuntimeTests/Unity.RuntimeTests.csproj");
}
