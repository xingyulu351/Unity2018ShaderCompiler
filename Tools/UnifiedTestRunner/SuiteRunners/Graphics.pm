package SuiteRunners::Graphics;

use parent SuiteRunner;
use parent FolderDeleter;

use warnings;
use strict;

use Dirs;
use LineTail;
use threads::shared;
use File::Spec::Functions qw (canonpath catdir catfile);
use File::Path qw (mkpath);
use TestPlan;
use English qw (-no_match_vars);
use lib Dirs::external_root();
use Archive::Zip;
use FileUtils qw (readAllLines);
use File::Copy::Recursive qw (dircopy);
use TestResult;
use Switch;
use UnityTestProtocol::Utils;
use Option;
use RunnerOptions;
use Carp qw (croak);
use BuildEngines::MSBuild;
use FileWatcher;
use UnityTestProtocol::Utils qw(:all);

my $root = Dirs::getRoot();
my $graphicsTestRunnerProj = canonpath("$root/Tests/Unity.GraphicsTestsRunner/Unity.GraphicsTestsRunner.csproj");
my $unifiedTestRunnerCSharpProj = canonpath("$root/Tests/Unity.UnifiedTestRunner/Unity.UnifiedTestRunner.csproj");

my $suiteOptions = [
    ['build-mode:s', 'Build mode', { hidden => 1, defaultValue => 'minimal' }],
    ['build-isolation:s', undef, { hidden => 1, defaultValue => 'enabled' }],
    [
        'autodoc-file:s',
        'Path to autodocs option file',
        {
            hidden => 1,
            defaultValue => "$root/Tests/Docs/docs/TestFrameworks/GraphicsTests/UnifiedTestRunner.Options.generated.md"
        }
    ],
    ['delete-player', 'Delete player if run was successful.'],
    ['open-report', 'Open HTML report in browser if test execution failed'],
];

sub new
{
    my ($pkg) = @_;
    my $this = $pkg->SUPER::new(
        suiteOptions => $suiteOptions,
        optionsGenProj => $unifiedTestRunnerCSharpProj,
        testRunnerLibProj => $graphicsTestRunnerProj,
    );
    return $this;
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

sub usage
{
    my ($this) = @_;
    return (
        description => "Tests rendering features aka 'graphics' tests",
        prerequisites => "perl build.pl repos fetch Tests/Unity.GraphicsTestsRunner/GfxTestProjectFolder",
        url => 'http://confluence.hq.unity3d.com/display/DEV/Graphics+Functional+Tests',
        examples => [

            { description => 'Run only tests with the substring \'001\' in them', command => '--testfiler=001' },
            {
                description => 'Run only tests with the \'Stereo\' configuration, and NOT the \'MultiPass\' configurations',
                command => '--configfilter=Stereo;!MultiPass'
            },
            { description => 'Run all tests on a built player', command => '--player-load-path=c:/temp/player' },
            { description => 'List tests for android platform', command => '--platform=android' },
            { description => 'Build player only', command => '--player-save-path=c:/temp/player --build-only' },
        ]
    );
}

sub getName
{
    return 'graphics';
}

sub getPlatform
{
    my ($this) = @_;
    return $this->getOptionValue('platform', 'standalone');
}

sub registerStaticArtifacts
{
    my ($this) = @_;
    $this->registerArtifacts($this->getHumanLogPath());
}

sub buildNativePluginDependencies
{
    my ($this) = @_;
    if ($^O =~ /mswin32|mswin64|cygwin/i)
    {
        my $msBuild = $this->_getMSBuild();
        my $nativePluginTestProj = canonpath(
            "$root/Tests/Unity.GraphicsTestsRunner/GfxTestProjectFolder/PluginSources/569-RenderingAndShaderNativePluginExtensions/GfxPluginTests.vcxproj");
        $msBuild->build(
            project => $nativePluginTestProj,
            config => 'Release',
            platform => 'x64',
            logfile => $this->msBuildLogFile('GfxPluginTests.vcxproj', 'Release-x64')
        );

        $msBuild->build(
            project => $nativePluginTestProj,
            config => 'Release',
            platform => 'Win32',
            logfile => $this->msBuildLogFile('GfxPluginTests.vcxproj', 'Release-Win32')
        );
    }
}

sub _getMSBuild
{
    my ($this) = @_;
    return new BuildEngines::MSBuild();
}

sub buildDependencies
{
    my ($this) = @_;

    # native plugin extensions test is disabled until building the plugin
    # does not require visual studio to be on the gfx test agents (case 903050)
    #$this->buildNativePluginDependencies();
    $this->cleanCSharpProjectsSln();
    $this->buildCSDependencies($graphicsTestRunnerProj, $unifiedTestRunnerCSharpProj);
    $this->{dependencies_built} = 1;
}

my $totalTests : shared = undef;
my $currentTestNumber : shared = 0;

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

sub doRun
{
    my ($this) = @_;
    $this->prepareBuildTarget($this->getPlatform());
    my $humanLog = $this->getHumanLogPath();
    my $machineLong = $this->getMachineLogPath();
    my @args = $this->prepareArgsForRun();

    Logger::minimal("Running " . $this->getName() . "tests Logs:\n\t$humanLog\n\t$machineLong");

    $this->{runnerExitCode} = $this->runDotNetProgram(
        program => $this->getPathToTestRunner(),
        programArgs => \@args,
        filesToWatch => [
            { file => $machineLong, callback => \&SuiteRunner::watchTestProtocolMessages, callbackArg => $this },
            { file => $humanLog, callback => \&Plugin::printLogMessage, callbackArg => $this },
        ]
    );

    my $reuseExistingPlayer = $this->getOptionValue('player-load-path');
    if (not defined($reuseExistingPlayer))
    {
        # if tests failed, do put player build into artifacts
        if ($this->{runnerExitCode} != 0)
        {
            $this->registerArtifact($this->playersRoot());
        }
        else
        {
            my $playerPath = $this->playersRoot();
            Logger::minimal("Delete the player from '$playerPath' after the successful run");
            if ($this->getOptionValue('delete-player'))
            {
                my $deleter = new FolderDeleter();
                $deleter->delete($this->playersRoot());
            }
            else
            {
                $this->registerArtifact($this->playersRoot());
            }
        }
    }

    $this->zipFailedImages();
    $this->doDirtyKatatanaCompatibilityTrick();
}

sub doBuildPlayer
{
    my ($this) = @_;
    $this->prepareBuildTarget($this->getPlatform());
    my @args = $this->prepareArgumentsForBuild();
    my $humanLog = $this->getHumanLogPath();
    $this->runDotNetProgram(
        program => $this->getPathToTestRunner(),
        programArgs => \@args,
        filesToWatch => [{ file => $humanLog, callback => \&Plugin::printLogMessage, callbackArg => $this },]
    );
    $this->registerArtifact($this->playersRoot());
}

sub prepareArgumentsForBuild
{
    my ($this) = @_;
    my @args = ();
    push(@args, "--suite=" . $this->getName());
    push(@args, "--build-only");
    $this->_addRunnerOptions(\@args);
    push(@args, "--no-stdout");
    return @args;
}

sub prepareArgsForRun
{
    my ($this) = @_;

    my @args = ();
    push(@args, "--suite=graphics");

    my $artifactsRoot = $this->artifactsFolder();
    push(@args, "--artifacts_path=$artifactsRoot");

    #https://favro.com/card/c564ede4ed3337f7b17986b6/Uni-26142
    my $resultsFolderName = $this->resultsFolderName();
    push(@args, "--resultsfolder=$resultsFolderName");
    push(@args, "--no-stdout");
    if ($this->getOptionValue('open-report'))
    {
        push(@args, '--open-report');
    }
    $this->_addRunnerOptions(\@args);
    return @args;
}

sub prepareArtifactsFolders
{
    my ($this) = @_;
    if ($this->{artifact_folders_prepared})
    {
        return;
    }
    $this->createArtifactsDirIfNotExistsOrCroak('UnifiedTestRunner');
    $this->{artifact_folders_prepared} = 1;
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
    my $scene = $this->getOptionValue('testfilter');
    my $platform = $this->getPlatform();
    my $testresultsxml = $this->testResultXmlFileName();
    my $testPlanFileName = $this->testPlanFileName();
    my @args = ();
    push(@args, "--list");
    push(@args, "--suite=" . $this->getName());
    $this->_addRunnerOptions(\@args);
    push(@args, "1>$testPlanFileName");
    return @args;
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

my $started : shared = 0;

sub onMessage
{
    my ($this, $msg) = @_;
    switch ($msg->{type})
    {
        case 'TestPlan' { $this->processTestPlanMessage($msg); }
        case 'Action' { $this->processActionMessage($msg); }
        case 'TestStatus' { $this->processTestStatusMessage($msg); }
        case 'ProcessInfo' { $this->processProcessInfoMessage($msg); }
    }
}

my $rawLogsWatches = {};

sub processProcessInfoMessage
{
    my ($this, $msg) = @_;
    my $status = 'unknown';
    if (isBeginMessage($msg))
    {
        $status = "Running";
    }
    else
    {
        $status = "Completed";
    }

    my $arguments = $msg->{arguments};
    if (defined($arguments))
    {
        Logger::minimal("$status $msg->{path} $arguments");
    }
    else
    {
        Logger::minimal("$status $msg->{path}");
    }

    my @processLogs = @{ $msg->{logs} };

    if (isBeginMessage($msg))
    {
        foreach my $log (@processLogs)
        {
            my $fileWatcher = new FileWatcher();
            $fileWatcher->watch(
                $log,
                sub
                {
                    my ($message) = @_;
                    SuiteRunner::watchTestProtocolMessages($message, $this, $log);
                }
            );
            $fileWatcher->start();
            $rawLogsWatches->{$log} = $fileWatcher;
        }
    }
    elsif (isEndMessage($msg))
    {
        foreach my $log (@processLogs)
        {
            $rawLogsWatches->{$log}->stop();
        }
    }
}

sub processActionMessage
{
    my ($this, $msg) = @_;
    my $progressPresenter = $this->getProgressPresenter();
    if (isBeginMessage($msg))
    {
        $progressPresenter->stepStarted(description => $msg->{description});
    }

    if (isEndMessage($msg))
    {
        $progressPresenter->stepDone(
            description => $msg->{description},
            duration => $msg->{duration}
        );
    }

    if (isEndMessage($msg) and hasErrors($msg))
    {
        $progressPresenter->stepError(
            description => $msg->{description},
            error => join("\n", @{ $msg->{errors} }),
            duration => $msg->{duration}
        );
    }
}

sub processTestStartMessage
{
    my ($this, $msg) = @_;
    ++$currentTestNumber;
    my $progressPresenter = $this->getProgressPresenter();
    $progressPresenter->stepStarted(
        progress => { current => $currentTestNumber, total => $totalTests },
        progressDetails => $msg->{name}
    );
}

sub processTestFinishMessage
{
    my ($this, $msg) = @_;
    my $progressPresenter = $this->getProgressPresenter();
    my $state = TestResult::stateToString($msg->{state});
    my $duration = $msg->{duration};
    $progressPresenter->stepDone(
        progress => { current => $currentTestNumber, total => $totalTests },
        progressDetails => "$msg->{name} $state",
        description => 'test',
        duration => $duration
    );
}

sub processTestPlanMessage
{
    my ($this, $msg) = @_;
    $this->{testPlan} = getTestPlanFromMessage($msg);
    $totalTests = $this->{testPlan}->getTotalTestCases();
}

sub msBuildLogFile
{
    my ($this, $project, $config) = @_;
    return $this->{artifactsNameBuilder}->getFullAtrifactFileName(
        'parent-type' => 'UnifiedTestRunner',
        type => "$project-$config-MSBuildLog",
        extension => 'txt'
    );
}

sub resultsFolderName
{
    my ($this) = @_;
    my $resultsFolder = $this->getOptionValue('resultsfolder');
    if (defined($resultsFolder))
    {
        return $resultsFolder;
    }
    return $this->{artifactsNameBuilder}->getFullFolderPathForArtifactType(type => 'Results');
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

sub playersRoot
{
    my ($this) = @_;
    my $playersRoot = $this->getOptionValue('player-save-path');
    if (defined($playersRoot))
    {
        return canonpath($playersRoot);
    }
    return $this->{artifactsNameBuilder}->getFullFolderPathForArtifactType(type => 'Players');
}

sub artifactsFolder
{
    my ($this) = @_;
    return $this->getArtifactsPath();
}

sub zipFailedImages
{
    my ($this) = @_;

    my $failedImagesDir = catdir($this->resultsFolderName(), "Failed");
    if (not -d $failedImagesDir)
    {
        return;
    }
    my $targetZip = $this->{artifactsNameBuilder}->getFullAtrifactFileName(
        'parent-type' => 'UnifiedTestRunner',
        type => 'FailedImages',
        extension => 'zip'
    );

    Logger::minimal("Zipping $failedImagesDir --> $targetZip");

    my $zip = new Archive::Zip();
    $zip->addTree($failedImagesDir);
    $zip->writeToFileNamed($targetZip);
    $this->registerArtifact($targetZip);
}

sub startNewIteration
{
    my ($this) = @_;
    $this->{artifact_folders_prepared} = 0;
}

sub doDirtyKatatanaCompatibilityTrick
{
    my ($this) = @_;
    if (not $ENV{UNITY_THISISABUILDMACHINE})
    {
        return;
    }
    if (-d $this->resultsFolderName())
    {
        $this->registerArtifact($this->resultsFolderName());
        my $destFolder = canonpath("$root/Tests/Unity.GraphicsTestsRunner/GfxTestProjectFolder/Results");
        if (not -d $destFolder)
        {
            mkpath($destFolder);
        }
        Logger::minimal("Copying failed images to $destFolder");
        dircopy($this->resultsFolderName(), $destFolder);
    }
}

sub getPathToTestRunner
{
    my ($this) = @_;
    return $this->_getAssemblyFullPath($unifiedTestRunnerCSharpProj);
}

1;
