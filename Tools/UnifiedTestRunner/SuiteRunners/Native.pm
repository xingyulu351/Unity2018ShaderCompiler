package SuiteRunners::Native;

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
use JSON;
use Option;
use RunnerOptions;
use Switch;
use MDHelpGenerator;

my $root = Dirs::getRoot();
my $nativeRunnerProj = canonpath("$root/Tests/Unity.NativeTestsRunner/Unity.NativeTestsRunner.csproj");
my $reporterProj = canonpath("$root/Tests/Unity.PerformanceTests.Reporter/Unity.PerformanceTests.Reporter.csproj");

sub new
{
    my ($pkg) = @_;

    my $suiteOptions = [
        [
            'category:s@',
            'Native tests category.',
            {
                defaultValue => ['Unit', 'Integration'],
                allowedValues => ['Unit', 'Integration', 'Performance', 'Stress', 'Regression']
            }
        ],

        #TODO overridie build  default value until it become, global default
        ['build-mode:s', 'Build mode', { hidden => 1, defaultValue => 'minimal' }],
        ['build-isolation:s', undef, { hidden => 1, defaultValue => 'enabled' }],
        [
            'autodoc-file:s',
            'Path to autodocs option file',
            {
                hidden => 1,
                defaultValue => "$root/Tests/Docs/docs/TestFrameworks/NativeTests/UnifiedTestRunner.Options.generated.md"
            }
        ],
        ['update-runner-options', 'Updates options json file', { hidden => 1 }],
    ];
    my @reporterOptions = RunnerOptions::readFromFile(_getReporterOptionsFileName());
    return $pkg->SUPER::new(
        suiteOptions => $suiteOptions,
        additionalOptions => \@reporterOptions,
        optionsGenProj => $nativeRunnerProj
    );
}

sub usage
{
    my ($this) = @_;
    return (
        description => 'Native tests',
        url => 'http://internaldocs.hq.unity3d.com/automation/TestFrameworks/NativeTests/About/index.html',
        examples => [
            { description => 'Run all unit tests for editor platform', command => '--platform=editor' },
            { description => 'Run all unit tests for android platform', command => '--platform=android' },
            {
                description => 'Run performance tests matching \'MemoryManager_\'',
                command => '--category=performance --testfilter=MemoryManager_'
            }
        ]
    );
}

sub getName
{
    my ($this) = @_;
    return 'native';
}

sub getReporterSuiteName
{
    my ($this) = @_;
    return 'Native';
}

sub getPlatform
{
    my ($this) = @_;
    return $this->getOptionValue('platform', 'editor');
}

sub updateAutomationDocs
{
    my ($this) = @_;
    my $helpFile = $this->getOptionValue('update-autodocs');
    my $options = $this->getOptions();
    my $text = OptionsHelpPrinter::getText(@$options);
    FileUtils::writeAllLines($helpFile, $text);
}

sub buildDependencies
{
    my ($this) = @_;
    if ($this->{dependencies_built})
    {
        return;
    }
    $this->cleanCSharpProjectsSln();
    my $proj = $this->buildCSDependencies($nativeRunnerProj);

    my $suiteName = $this->getReporterSuiteName();
    my $configName = $this->getOptionValue('config');
    my $category = $this->getOptionValue('category');
    if (PerformanceReporter::ShouldReportPerformanceData($suiteName, $configName, $category))
    {
        $this->buildCSProject($reporterProj);
    }
    $this->{dependencies_built} = 1;
}

sub prepareArtifactsFolders
{
    my ($this) = @_;
    if ($this->{artifact_folders_prepared})
    {
        return;
    }
    $this->createArtifactsDirIfNotExistsOrCroak('NativeTestsRunner');
    $this->createArtifactsDirIfNotExistsOrCroak('UnifiedTestRunner');
    $this->createArtifactsDirIfNotExistsOrCroak('Unity');

    my $suiteName = $this->getReporterSuiteName();
    my $configName = $this->getOptionValue('config');
    my $category = $this->getOptionValue('category');
    if (PerformanceReporter::ShouldReportPerformanceData($suiteName, $configName, $category))
    {
        $this->createArtifactsDirIfNotExistsOrCroak('Unity.PerformanceTests.Reporter');
    }
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

sub writeSuiteData
{
    my ($this) = @_;
    my $category = $this->getOptionValue('category');
    if (defined($category) and grep { $_ =~ /^performance$/i } @$category)
    {
        my ($this, $suiteResult) = @_;
        my $performanceDataFile = $this->performanceDataFileName();
        if (-e $performanceDataFile)
        {
            my $json = join(q (), FileUtils::readAllLines($performanceDataFile));
            PerformanceDataPostProcessor::embedPerformanceData(jsonToObj($json), $suiteResult);
        }
    }
}

sub doRun
{
    my ($this) = @_;
    my @cmdArgs = $this->prepareArgs();
    $this->prepareBuildTarget($this->getPlatform());
    my $root = Dirs::getRoot();
    my $nativeSuiteRunner = $this->_getAssemblyFullPath($nativeRunnerProj);
    my $logFile = $this->getLogFileName();

    my $stdErr = $this->stdErrFileName();
    my $stdOut = $this->stdOutFileName();
    my $suite = $this->getName();
    my $platform = $this->getPlatform();

    Logger::minimal("Running $suite $platform. Test runner logs:\n\t$stdOut\n\t$stdErr");
    $this->{runnerExitCode} = $this->runDotNetProgram(
        program => $nativeSuiteRunner,
        programArgs => \@cmdArgs,
        filesToWatch => [
            { file => $logFile, callback => \&SuiteRunner::watchTestProtocolMessages, callbackArg => $this },
            { file => $logFile, callback => \&Plugin::printLogMessage, callbackArg => $this },
            { file => $stdOut, callback => \&SuiteRunner::watchTestProtocolMessages, callbackArg => $this },
            { file => $stdErr, callback => \&Plugin::printLogErrorMessage, callbackArg => $this },
        ]
    );

    # Dont report performance data in QV until all tests are stable enough
    if (defined($this->getOptionValue('dontreportperformancedata')))
    {
        return;
    }

    my $suiteName = $this->getReporterSuiteName();
    my $configName = $this->getOptionValue('config');
    my $category = $this->getOptionValue('category');
    if (PerformanceReporter::ShouldReportPerformanceData($suiteName, $configName, $category))
    {
        my $reporterStdOut = $this->reporterStdOutFileName();
        my $reporterStdErr = $this->reporterStdErrFileName();
        $this->registerArtifacts($reporterStdOut, $reporterStdErr);

        $this->_reportPerformanceData();
    }
}

sub prepareArgs
{
    my ($this) = @_;

    my @cmdArgs = ();

    my $platform = $this->getOptionValue('platform');
    if (defined($platform))
    {
        push(@cmdArgs, "--platform=$platform");
    }
    else
    {
        push(@cmdArgs, "--platform=editor");
    }

    my $category = $this->getOptionValue('category');
    if (defined($category))
    {
        push(@cmdArgs, map { "--category=$_" } @$category);
    }

    push(@cmdArgs, '--testresultsxml=' . $this->testResultXmlFileName());

    my $logFile = $this->getLogFileName();
    push(@cmdArgs, "--logfile=$logFile");

    my @directArgs = $this->parseDirectRunnerArgs();
    push(@cmdArgs, @directArgs);

    my $artifactsRoot = $this->artifactsFolder();
    push(@cmdArgs, "--artifactspath=$artifactsRoot");

    $this->_addRunnerOptions(\@cmdArgs);

    my $stdOut = $this->stdOutFileName();
    push(@cmdArgs, "1>$stdOut");

    my $stdErr = $this->stdErrFileName();
    push(@cmdArgs, "2>$stdErr");

    my $tool = $this->getTool();
    if ($tool)
    {
        @cmdArgs = $tool->decorate(@cmdArgs);
    }

    return @cmdArgs;
}

sub getArtifactsMap
{
    my ($this) = @_;
    my $stdOut = $this->stdOutFileName();
    return ArtifactsMapBuilder::fromNativeSuiteLog($stdOut, $this->getArtifactsPath());
}

sub afterRun
{
    my ($this) = @_;
    $this->SUPER::afterRun();

    if (FileUtils::fileExists($this->getLogFileName()))
    {
        $this->registerArtifacts($this->getLogFileName());
    }

    my $artifactsMap = $this->getArtifactsMap();
    if (not $artifactsMap)
    {
        return;
    }
    foreach my $art ($artifactsMap->getArtifacts())
    {
        $this->registerArtifacts($art);
    }
}

sub parseDirectRunnerArgs
{
    my ($this) = @_;
    my $argsString = $this->getOptionValue('direct-runner-args');
    if (not defined($argsString))
    {
        my (@empty_result);
        return @empty_result;
    }

    $argsString =~ s/^["']//s;
    $argsString =~ s/"["']$//s;
    Readonly my $SPACE => q ( );
    return split($SPACE, $argsString);
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

sub testResultXmlFileName
{
    my ($this) = @_;
    return $this->testResultXmlFileNameImpl('NativeTestsRunner');
}

sub stdOutFileName
{
    my ($this) = @_;
    return $this->{artifactsNameBuilder}->getFullAtrifactFileName(
        'parent-type' => 'NativeTestsRunner',
        type => 'stdout',
        extension => 'txt'
    );
}

sub stdErrFileName
{
    my ($this) = @_;
    return $this->{artifactsNameBuilder}->getFullAtrifactFileName(
        'parent-type' => 'NativeTestsRunner',
        type => 'stderr',
        extension => 'txt'
    );
}

sub getLogFileName
{
    my ($this) = @_;
    my $logFile = $this->getOptionValue('logfile');
    if (defined($logFile))
    {
        return canonpath($logFile);
    }

    return $this->{artifactsNameBuilder}->getFullAtrifactFileName(
        'parent-type' => 'Unity',
        type => 'log',
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

sub artifactsFolder
{
    my ($this) = @_;
    return $this->getArtifactsPath();
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
        programArgs => \@args,
        filesToWatch => []
    );

    $this->{testPlan} = readTestPlanFromFile($this->getLogFileName());
    return $this->{testPlan};
}

sub prepareArgsForList
{
    my ($this) = @_;
    my $testPlanFileName = $this->testPlanFileName();

    my @args = ();
    push(@args, "--list");

    my $platform = $this->getOptionValue('platform');
    if (defined($platform))
    {
        push(@args, "--platform=$platform");
    }
    else
    {
        push(@args, "--platform=editor");
    }

    my $category = $this->getOptionValue('category');
    if (defined($category))
    {
        push(@args, map { "--category=$_" } @$category);
    }

    my $filter = $this->getOptionValue('testfilter');
    if (defined($filter))
    {
        push(@args, "--testfilter=$filter");
    }
    my $logFile = $this->getLogFileName();
    push(@args, "--logfile=$logFile");
    $this->_addRunnerOptions(\@args);
    my $stdOut = $this->stdOutFileName();
    push(@args, "1>$stdOut");

    my $stdErr = $this->stdErrFileName();
    push(@args, "2>$stdErr");
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

my $totalNumberOfTests : shared = undef;

sub readTestPlanFromFile
{
    my ($fileName) = @_;
    my $msg = UnityTestProtocol::Utils::getFirst(fileName => $fileName, predicate => sub { return $_[0]->{type} eq 'TestPlan'; });
    my $testPlan = TestPlan::getTestPlanFromTestProtocolMessage($msg);
    $totalNumberOfTests = $testPlan->getTotalTestCases();
    return $testPlan;
}

sub getPathToTestRunner
{
    my ($this) = @_;
    return $this->_getAssemblyFullPath($nativeRunnerProj);
}

sub getPathToReporter
{
    my ($this) = @_;
    return $this->_getAssemblyFullPath($reporterProj);
}

{    #thread scope

    sub onMessage
    {
        my ($this, $msg) = @_;
        switch ($msg->{type})
        {
            case 'TestPlan' { $this->processTestPlanMessage($msg); }
            case 'TestStatus' { $this->processTestStatusMessage($msg); }
        }
    }

    sub processTestPlanMessage
    {
        my ($this, $msg) = @_;
        my $testPlan = TestPlan::getTestPlanFromTestProtocolMessage($msg);
        $totalNumberOfTests = $testPlan->getTotalTestCases();
    }

    my $started : shared = 0;
    my $currentTestNumber : shared = 0;

    sub processTestStartMessage
    {
        my ($this, $msg) = @_;
        ++$currentTestNumber;
        my $progressPresenter = $this->getProgressPresenter();
        $progressPresenter->stepStarted(
            progress => { current => $currentTestNumber, total => $totalNumberOfTests },
            progressDetails => $msg->{name}
        );
    }
}

1;
