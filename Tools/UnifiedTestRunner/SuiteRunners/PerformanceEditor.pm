package SuiteRunners::PerformanceEditor;

use parent SuiteRunner;

use warnings;
use strict;

use File::Spec::Functions qw (canonpath catfile);
use TestResult;
use SuiteResult;
use threads::shared;
use ProgressTracker;
use SuiteRunnerArtifactsNameBuilder;
use Dirs;
use lib Dirs::external_root();
use Readonly;
use PerformanceDataPostProcessor;
use JSON;
use FileUtils;
use RunnerOptions;
use TestPlan;
use UnityTestProtocol::Utils;

my $root = Dirs::getRoot();
my $runnerProj = canonpath("$root/Tests/Unity.PerformanceTests.Runner/Unity.PerformanceTests.Runner.csproj");
my $reporterProj = canonpath("$root/Tests/Unity.PerformanceTests.Reporter/Unity.PerformanceTests.Reporter.csproj");

my $suiteOptions = [
    ['build-mode:s', 'Build mode', { hidden => 1, defaultValue => 'minimal' }],
    ['build-isolation:s', undef, { hidden => 1, defaultValue => 'enabled' }],
    {
        prototypes => ['testresultsxml:s', 'testresultsjson:s']
    },
    ['buildtarget:s@', "Prepare player support, ex: Android"],
];

sub new
{
    my ($pkg) = @_;
    my @reporterOptions = RunnerOptions::readFromFile(_getReporterOptionsFileName());
    return $pkg->SUPER::new(
        suiteOptions => $suiteOptions,
        additionalOptions => \@reporterOptions,
        optionsGenProj => $runnerProj
    );
}

sub usage
{
    my ($this) = @_;
    return (
        description => "Performance Editor Tests",
        prerequisites => "perl build.pl repos fetch Tests/PerformanceTestAssets",
        url => 'https://confluence.hq.unity3d.com/display/DEV/Performance+Editor+Tests',
        examples => [{ description => 'Run asset import tests 10 times', command => '--executioncount=10 --testfilter=AssetsImport' },]
    );
}

sub getName
{
    my ($this) = @_;
    return 'performance-editor';
}

sub getReporterSuiteName
{
    my ($this) = @_;
    return 'Editor';
}

sub getPlatform
{
    my ($this) = @_;
    return $this->getOptionValue('platform', 'editor');
}

sub prepareBuildTargets
{
    my ($this) = @_;
    my $platforms = $this->getOptionValue('buildtarget');
    if (defined($platforms))
    {
        foreach my $p (@{$platforms})
        {
            $this->prepareBuildTarget($p);
        }
    }
}

sub _getTestAssemblyFullPath
{
    my ($this) = @_;
    my $proj = "$root/Tests/Unity.PerformanceTests/Unity.PerformanceTests.csproj";
    return $this->_getAssemblyFullPath($proj);
}

sub buildDependencies
{
    my ($this) = @_;
    if ($this->{dependencies_built})
    {
        return;
    }
    $this->buildCSProject(canonpath("$root/Tests/Unity.PerformanceTests/Unity.PerformanceTests.csproj"));
    $this->buildCSProject($runnerProj);
    $this->buildCSProject($reporterProj);
    $this->{dependencies_built} = 1;
}

sub prepareArtifactsFolders
{
    my ($this) = @_;
    if ($this->{artifact_folders_prepared})
    {
        return;
    }
    $this->createArtifactsDirIfNotExistsOrCroak('Unity.PerformanceTests.Runner');
    $this->createArtifactsDirIfNotExistsOrCroak('Unity.PerformanceTests.Reporter');
    $this->createArtifactsDirIfNotExistsOrCroak('UnifiedTestRunner');
    $this->createArtifactsDirIfNotExistsOrCroak('Unity');
    $this->{artifact_folders_prepared} = 1;
}

sub startNewIteration
{
    my ($this) = @_;
    $this->{artifact_folders_prepared} = 0;
}

sub registerStaticArtifacts
{
    my ($this) = @_;
    my $stdOut = $this->stdOutFileName();
    my $stdErr = $this->stdErrFileName();
    $this->registerArtifacts($stdOut);
    $this->registerArtifacts($stdErr);
}

sub doRun
{
    my ($this) = @_;
    my @cmdArgs = $this->prepareArgs();

    $this->prepareBuildTarget($this->getPlatform());
    $this->prepareBuildTargets();

    my $stdOut = $this->stdOutFileName();
    my $stdErr = $this->stdErrFileName();
    $this->{testPlan} = $this->getTestPlan();
    $this->{runnerExitCode} = $this->runDotNetProgram(
        program => $this->getPathToTestRunner(),
        programArgs => \@cmdArgs,
        filesToWatch => [
            { file => $stdOut, callback => \&printProgressMessage, callbackArg => $this },
            { file => $stdOut, callback => \&Plugin::printLogMessage, callbackArg => $this },
            { file => $stdErr, callback => \&Plugin::printLogErrorMessage, callbackArg => $this },
        ]
    );
    $this->registerArtifact($this->artifactsFolder());

    # Dont report performance data in QV until all tests are stable enough
    my $category = $this->getOptionValue('category');
    if (defined($category) and $category =~ /^QV$/)
    {
        return;
    }
    my $reporterStdOut = $this->reporterStdOutFileName();
    my $reporterStdErr = $this->reporterStdErrFileName();
    $this->registerArtifacts($reporterStdOut, $reporterStdErr);
    $this->_reportPerformanceData();
}

sub prepareArgs
{
    my ($this) = @_;

    my @cmdArgs = ();

    my $artifactsRoot = $this->artifactsFolder();
    push(@cmdArgs, "--artifactspath=$artifactsRoot");

    push(@cmdArgs, '--testresultsxml=' . $this->testResultXmlFileName());

    my $testAssembly = $this->_getTestAssemblyFullPath();
    push(@cmdArgs, "--assemblywithtests=$testAssembly");

    $this->_addRunnerOptions(\@cmdArgs);

    my $stdOut = $this->stdOutFileName();
    push(@cmdArgs, "1>$stdOut");

    my $stdErr = $this->stdErrFileName();
    push(@cmdArgs, "2>$stdErr");

    return @cmdArgs;
}

sub artifactsFolder
{
    my ($this) = @_;
    return $this->{artifactsNameBuilder}->getFullFolderPathForArtifactType(type => 'Artifacts');
}

sub editorLogsDir
{
    my ($this) = @_;
    my $editorLogsDir = $this->getOptionValue('editorlogsdir');
    if (defined($editorLogsDir))
    {
        return $editorLogsDir;
    }
    return $this->getSuiteRunArtifactsRoot();
}

sub testResultXmlFileName
{
    my ($this) = @_;
    return $this->testResultXmlFileNameImpl('Unity.PerformanceTests.Runner');
}

sub writeSuiteData
{
    my ($this, $suiteResult) = @_;
    my $performanceDataFile = $this->performanceDataFileName();
    if (-e $performanceDataFile)
    {
        my $json = join(q (), FileUtils::readAllLines($performanceDataFile));
        PerformanceDataPostProcessor::embedPerformanceData(jsonToObj($json), $suiteResult);
    }
}

sub stdOutFileName
{
    my ($this) = @_;
    return $this->{artifactsNameBuilder}->getFullAtrifactFileName(
        'parent-type' => 'Unity.PerformanceTests.Runner',
        type => 'stdout',
        extension => 'txt'
    );
}

sub stdErrFileName
{
    my ($this) = @_;
    return $this->{artifactsNameBuilder}->getFullAtrifactFileName(
        'parent-type' => 'Unity.PerformanceTests.Runner',
        type => 'stderr',
        extension => 'txt'
    );
}

sub reporterStdOutFileName
{
    my ($this) = @_;
    return $this->{artifactsNameBuilder}->getFullAtrifactFileName(
        'parent-type' => 'Unity.PerformanceTests.Reporter',
        type => 'stdout',
        extension => 'txt'
    );
}

sub reporterStdErrFileName
{
    my ($this) = @_;
    return $this->{artifactsNameBuilder}->getFullAtrifactFileName(
        'parent-type' => 'Unity.PerformanceTests.Reporter',
        type => 'stderr',
        extension => 'txt'
    );
}

sub performanceDataFileName
{
    my ($this) = @_;
    my ($path) = $this->getOptionValue('outputjson');
    if ($path)
    {
        return $path;
    }
    return $this->{artifactsNameBuilder}->getFullAtrifactFileName(
        'parent-type' => 'Unity.PerformanceTests.Reporter',
        type => 'perfdata',
        extension => 'json'
    );
}

sub getTestPlan
{
    my ($this) = @_;

    if (defined($this->{testPlan}))
    {
        return $this->{testPlan};
    }

    my $testPlanFileName = $this->testPlanFileName();
    truncate($testPlanFileName, 0);
    my @args = $this->prepareArgsForList();
    $this->runDotNetProgram(
        program => $this->getPathToTestRunner(),
        programArgs => \@args
    );

    $this->{testPlan} = readTestPlanFromFile($testPlanFileName);
    return $this->{testPlan};
}

sub prepareArgsForList
{
    my ($this) = @_;
    my $testPlanFileName = $this->testPlanFileName();

    my @args = ();
    push(@args, "--list");

    my $filter = $this->getOptionValue('testfilter');
    if (defined($filter))
    {
        push(@args, "--testfilter=$filter");
    }
    my $testAssembly = $this->_getTestAssemblyFullPath();
    push(@args, "--assemblywithtests=$testAssembly");
    $this->_addRunnerOptions(\@args);
    push(@args, "1>$testPlanFileName");
    return @args;
}

sub testPlanFileName
{
    my ($this) = @_;
    return $this->{artifactsNameBuilder}->getFullAtrifactFileName(
        'parent-type' => 'UnifiedTestRunner',
        type => 'TestPlan',
        extension => 'txt'
    );
}

sub readTestPlanFromFile
{
    my ($fileName) = @_;
    my $testPlan = new TestPlan();
    my $msg = UnityTestProtocol::Utils::getFirst(
        fileName => $fileName,
        predicate => sub { return $_[0]->{type} eq 'TestPlan'; }
    );
    return getTestPlanFromMessage($msg);
}

sub getTestPlanFromMessage
{
    my ($msg) = @_;
    my $testPlan = new TestPlan();
    if (not defined($msg))
    {
        return $testPlan;
    }
    foreach my $test (@{ $msg->{tests} })
    {
        my $testPlanItem = TestPlanItem->new(name => $test, testCaseCount => 1);
        $testPlan->registerTest($testPlanItem);
    }
    return $testPlan;
}

sub getPathToTestRunner
{
    my ($this) = @_;
    return $this->_getAssemblyFullPath($runnerProj);
}

sub getPathToReporter
{
    my ($this) = @_;
    return $this->_getAssemblyFullPath($reporterProj);
}

sub updateRunnerHelp
{
    my ($this) = @_;
    if ($this->{helpUpdated})
    {
        return (undef, 0);
    }
    my (undef, $exitCode) = $this->SUPER::updateRunnerHelp();
    if ($exitCode == 0)
    {
        my $reporterOptionsFileName = _getReporterOptionsFileName();
        return $this->_doUpdateRunnerHelp($reporterProj, $reporterOptionsFileName);
    }
}

sub _getReporterOptionsFileName
{
    return canonpath(catfile(Dirs::UTR_root(), 'Configs', "Unity.PerformanceTests.Reporter.Options.json"));
}

{    #thread scope
    my $progressTracker = undef;
    my $started = 0;
    my $currentTestNumber = 0;
    my $totalTests;

    sub printProgressMessage
    {
        my ($line, $this) = @_;
        if ($started == 0)
        {
            my $progressPresenter = $this->getProgressPresenter();
            $started = 1;
            $totalTests = $this->{testPlan}->getTotalTestCases();
            my $stdOut = $this->stdOutFileName();
            my $stdErr = $this->stdErrFileName();
            my $name = $this->getName();
            Logger::minimal("Running $name tests. Logs:\n\t$stdOut\n\t$stdErr");
        }

        my $isEof = not defined($line);
        if ($isEof)
        {
            return;
        }

        my ($test) = ($line =~ m/Running test case: (\w+.*)/);
        if ($test)
        {
            ++$currentTestNumber;
            my $progressPresenter = $this->getProgressPresenter();
            $progressPresenter->stepStarted(
                progress => { current => $currentTestNumber, total => $totalTests },
                progressDetails => $test
            );
        }
    }
}

1;
