package RuntimeBase;

use parent SuiteRunner;

use warnings;
use strict;

use File::Basename qw (fileparse);
use File::Path qw (mkpath);
use File::Spec::Functions qw (canonpath);
use FileUtils;
use Carp qw (croak);
use Dirs;

my $root = Dirs::getRoot();

sub stdOutFileName
{
    return allInOneRunnerStdOutFileName(@_);
}

sub stdErrFileName
{
    return allInOneRunnerStdErrFileName(@_);
}

sub testResultXmlFileName
{
    return allInOneRunnerTestResultXmlFile(@_);
}

sub allInOneRunnerTestResultXmlFile
{
    my ($this) = @_;
    return $this->testResultXmlFileNameImpl('AllInOneRunner');
}

sub allInOneRunnerStdOutFileName
{
    my ($this) = @_;
    return $this->{artifactsNameBuilder}->getFullAtrifactFileName(
        'parent-type' => 'AllInOneRunner',
        type => 'stdout',
        extension => 'txt'
    );
}

sub allInOneRunnerStdErrFileName
{
    my ($this) = @_;
    return $this->{artifactsNameBuilder}->getFullAtrifactFileName(
        'parent-type' => 'AllInOneRunner',
        type => 'stderr',
        extension => 'txt'
    );
}

sub projectPath
{
    my ($this) = @_;
    my $projectPath = $this->getOptionValue('projectpath');
    if (defined($projectPath))
    {
        return canonpath($projectPath);
    }
    return $this->{artifactsNameBuilder}->getFullFolderPathForArtifactType(type => 'UnityProject');
}

sub defaultPlayerPath
{
    my ($this) = @_;
    return $this->{artifactsNameBuilder}->getFullFolderPathForArtifactType(type => 'PlayerBin');
}

sub artifactsFolder
{
    my ($this) = @_;
    my $artifactsPath = $this->getOptionValue('unity-artifacts-folder');
    if (defined($artifactsPath))
    {
        return canonpath($artifactsPath);
    }
    return $this->getArtifactsPath();
}

sub logFilePath
{
    my ($this) = @_;

    my $logFilePath = $this->getOptionValue('logfilepath');
    if (defined($logFilePath))
    {
        return canonpath($logFilePath);
    }
    return undef;
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
    $this->registerArtifacts($stdOut, $stdErr);
}

sub prepareArtifactsFolders
{
    my ($this) = @_;
    if ($this->{artifact_folders_prepared})
    {
        return;
    }
    $this->createArtifactsDirIfNotExistsOrCroak('AllInOneRunner');
    $this->createArtifactsDirIfNotExistsOrCroak('UnifiedTestRunner');
    $this->createArtifactsDirIfNotExistsOrCroak('Unity');

    my $logFilePath = $this->logFilePath();
    if (defined($logFilePath))
    {
        my $dirName = FileUtils::getDirName($logFilePath);
        my $res = mkpath($dirName);
        if (not $res)
        {
            croak("Can not create dir: $dirName");
        }
    }

    $this->{artifact_folders_prepared} = 1;
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
        my $testPlanFileName = $this->testPlanFileName();
        $this->{testPlan} = $this->buildTestPlan($testPlanFileName);
    }

    # Can be overriden in subclasses:
    $this->{testPlan} = $this->adjustTestPlan($this->{testPlan});

    return $this->{testPlan};
}

sub buildTestPlan
{
    my ($this, $testPlanFileName) = @_;
    truncate($testPlanFileName, 0);
    my @args = $this->prepareArgsForListTests();
    my $exitCode = $this->runDotNetProgram(
        program => $this->getPathToTestRunner(),
        programArgs => \@args
    );
    if ($exitCode != 0)
    {
        croak($this->getPathToTestRunner() . " exited with $exitCode");
    }
    my $result = $this->readTestPlanFromFile($testPlanFileName);
    unlink($testPlanFileName);
    return $result;
}

sub adjustTestPlan
{
    my ($this, $testPlan) = @_;
    return $testPlan;
}

sub getPathToTestRunner
{
    my ($this) = @_;
    return canonpath("$root/Tests/bin/Debug/Unity.RuntimeTests.AllIn1Runner.exe");
}

sub readTestPlanFromFile
{
    my ($this, $testPlanFileName) = @_;

    my $fh = $this->_openFileForRead($testPlanFileName);
    my $testPlan = new TestPlan();
    while (<$fh>)
    {
        my $line = $_;
        chomp($line);
        if ($line =~ m/^\b[a-z0-9_]+\b$/i)
        {
            my $testPlanItem = TestPlanItem->new(name => $line, testCaseCount => 1);
            $testPlan->registerTest($testPlanItem);
        }
    }
    close($fh);
    return $testPlan;
}

sub beforeRun
{
    my ($this) = @_;
    $this->writeTestPlan($this->testPlanFileName(), $this->getTestPlan());
}

{
    # thread scope
    my $progressTracker = undef;
    my $totalTests;
    my $started = 0;
    my $currentTestNumber = 0;

    sub printProgressMessage
    {
        my ($line, $this) = @_;

        if ($started == 0)
        {
            my $stdOut = $this->stdOutFileName();
            my $stdErr = $this->stdErrFileName();
            my $name = $this->getName();
            $started = 1;
            $totalTests = $this->{testPlan}->getTotalTestCases();
            Logger::minimal("Running $name tests. Logs:\n\t$stdOut\n\t$stdErr");
            Logger::minimal("Preparing assets...");
        }

        my $isEof = not defined($line);
        if ($isEof)
        {
            return;
        }

        my ($test) = ($line =~ m /^##teamcity\[testStarted\sname=\'(.*)\'/);
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
