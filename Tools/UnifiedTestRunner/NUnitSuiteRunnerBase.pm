package NUnitSuiteRunnerBase;

use parent SuiteRunner;

use warnings;
use strict;

use File::Spec::Functions qw (catfile canonpath);
use File::Basename qw (fileparse);
use File::Path qw (mkpath);

use ArtifactsFolderDeleterGuard;
use FolderDeleter;
use TestPlan;
use TestPlanItem;
use ProgressTracker;
use Carp qw (croak);
use Dirs;
use lib Dirs::external_root();
use Switch;
use UnityTestProtocol::Utils qw(:all);

sub readTestPlanFromFile
{
    my ($filename) = @_;
    return TestPlan::loadTestPlanFromTestListerOutput($filename);
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

sub runNUnitProject
{
    my ($this, %args) = @_;

    my $testPlan = $this->getTestPlan();
    if ($testPlan->getTotalTests() == 0)
    {
        Logger::error("No tests have been selected to run. Check if you specified correct '-testfilter' and '-testtarget'");
        return 1;
    }

    my @cmdArgs;
    push(@cmdArgs, '-labels=All');

    my $testresultsxml = canonpath($this->testResultXmlFileName());
    my $testResultsFormat = $this->testResultXmlFormat();

    my $testResultsArgument = "-result=$testresultsxml";
    if (defined($testResultsFormat) && $testResultsFormat ne "")
    {
        $testResultsArgument = "-result=\"$testresultsxml;format=$testResultsFormat\"";
    }
    push(@cmdArgs, $testResultsArgument);

    push(@cmdArgs, "-inprocess");
    my $testPlanFileName = $this->testPlanFileName();
    push(@cmdArgs, "-testlist=$testPlanFileName");

    my $stdOut = $this->stdOutFileName();
    push(@cmdArgs, "1>$stdOut");

    my $stdErr = $this->stdErrFileName();
    push(@cmdArgs, "2>$stdErr");

    $this->_addRunnerOptions($args{directRunnerArgs});

    if (defined($args{directRunnerArgs}))
    {
        my @existingArgs = @{ $args{directRunnerArgs} };
        unshift(@cmdArgs, @existingArgs);
    }

    #TODO: get rid of it. for now keep it for historical reasons.
    #TODO: move to Mono.pm
    my $environment = $this->getEnvironment();
    if ($environment->getName() eq 'mono')
    {
        unshift(@cmdArgs, '-domain=None');
    }

    my $privateBinPaths = $this->getOptionValue('privatebinpath');
    if (defined($privateBinPaths))
    {
        push(@cmdArgs, '--privatebinpath=' . join(';', @${privateBinPaths}));
    }
    $args{filesToWatch} = [
        { file => $stdOut, callback => \&printProgressMessage, callbackArg => $this },
        { file => $stdOut, callback => \&Plugin::printLogMessage, callbackArg => $this },
        { file => $stdErr, callback => \&Plugin::printLogErrorMessage, callbackArg => $this },
    ];

    $args{directRunnerArgs} = \@cmdArgs;
    $environment->runNUnitProject(%args);

    # Force return 0 exit code. Nunit-console assumes wide 32 bit windows exit code.
    # which contains sum of fail, error, inconclusive and not runnable tests.
    # In posix environment exit code should fit into 8 bits.

    # It would be possible to rely on 0 code but it could mean 256 failed tests.
    # Moreover positive number could mean success, in case
    # case if there are not runnable tests.

    # There are pretty strict conditions to exit with 0 anyway,
    # so let upper layers to decide in this case.
    return 0;
}

{    #thread scope
    my $currentTestNumber : shared = 0;
    my $totalTestCases : shared = 0;
    my $started : shared = 0;

    sub printProgressMessage
    {
        my ($line, $this, $filename) = @_;
        if ($started == 0)
        {
            $totalTestCases = $this->{testPlan}->getTotalTestCases();
            my $stdOut = $this->stdOutFileName();
            my $stdErr = $this->stdErrFileName();
            my $name = $this->getName();
            $started = 1;
            Logger::minimal("Running $name tests. Logs:\n\t$stdOut\n\t$stdErr\n");
        }

        my $isEof = not defined($line);
        if ($isEof)
        {
            return;
        }

        my $msg = UnityTestProtocol::Utils::getMessage($line);
        if ($msg)
        {
            $this->onMessage($msg, $filename);
            RemoteEventLogger::log($msg);
        }
    }

    sub onMessage
    {
        my ($this, $msg, $filename) = @_;
        if (isTestBegin($msg))
        {
            $this->processTestStartMessage($msg);
        }
        elsif (isTestEnd($msg))
        {
            $this->processTestFinishMessage($msg);
        }
    }

    sub processTestStartMessage
    {
        my ($this, $msg) = @_;
        ++$currentTestNumber;
        my $progressPresenter = $this->getProgressPresenter();
        $progressPresenter->stepStarted(
            progress => { current => $currentTestNumber, total => $totalTestCases },
            progressDetails => $msg->{name}
        );
    }
}

sub getNUnitArgs
{
    return ();
}

sub stdOutFileName
{
    return nunitConsoleStdOutFileName(@_);
}

sub stdErrFileName
{
    return nunitConsoleStdErrFileName(@_);
}

sub nunitConsoleStdOutFileName
{
    my ($this) = @_;
    return $this->{artifactsNameBuilder}->getFullAtrifactFileName(
        'parent-type' => 'NUnitConsole',
        type => 'stdout',
        extension => 'txt'
    );
}

sub nunitConsoleStdErrFileName
{
    my ($this) = @_;
    return $this->{artifactsNameBuilder}->getFullAtrifactFileName(
        'parent-type' => 'NUnitConsole',
        type => 'stderr',
        extension => 'txt'
    );
}

sub testResultXmlFileName
{
    my ($this) = @_;
    return $this->nunitConsoleTestResultXmlFile();
}

sub testResultXmlFormat
{
    my ($this) = @_;
    return $this->nunitConsoleTestResultXmlFormat();
}

sub nunitConsoleTestResultXmlFile
{
    my ($this) = @_;
    return $this->testResultXmlFileNameImpl('NUnitConsole');
}

sub nunitConsoleTestResultXmlFormat
{
    my ($this) = @_;
    return $this->testResultXmlFormatImpl('NUnitConsole');
}

sub prepareArtifactsFolders
{
    my ($this) = @_;
    if ($this->{artifact_folders_prepared})
    {
        return;
    }
    $this->createArtifactsDirIfNotExistsOrCroak('NUnitConsole');
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

    my $root = Dirs::getRoot();

    my @cmdArgs = ();

    my @includes = $this->getIncludes();
    if (scalar(@includes) > 0)
    {
        push(@cmdArgs, "--includes=" . join(',', @includes));
    }

    my @excludes = $this->getExcludes();
    if (scalar(@excludes) > 0)
    {
        push(@cmdArgs, "--excludes=" . join(',', @excludes));
    }

    my @dlls = $this->getDllNames();
    foreach my $dll (@dlls)
    {
        push(@cmdArgs, "--assemblywithtests=$dll");
    }

    my $filter = $this->getOptionValue('testfilter');
    push(@cmdArgs, "--testfilter=$filter") if defined($filter);

    my $testPlanFileName = $this->testPlanFileName();
    push(@cmdArgs, "1>$testPlanFileName");

    my @monoArgs = $this->getMonoArgs();

    $this->runDotNetProgram(
        program => canonpath("$root/Tests/Unity.TestLister/bin/Debug/Unity.TestLister.exe"),
        programArgs => \@cmdArgs,
        monoArgs => \@monoArgs
    );

    $this->{testPlan} = NUnitSuiteRunnerBase::readTestPlanFromFile($testPlanFileName);

    unlink($testPlanFileName);
    return $this->{testPlan};
}

sub buildNUnitTestProtocolAddin
{
    my ($this) = @_;
    my $logFile = $this->{artifactsNameBuilder}->getFullAtrifactFileName(
        'parent-type' => 'UnifiedTestRunner',
        type => 'TestProtocol.NUnit.Addin-BuildLog',
        extension => 'txt'
    );
    my $program = catfile(Dirs::getRoot(), 'Tests', 'Unity.TestProtocol.NUnit', 'build');
    my $exitCode = $this->callSystem($program, ">$logFile");
    if ($exitCode != 0)
    {
        croak("failed to build Unity.TestProtocol.NUnit. see $logFile");
    }
}

sub startNewIteration
{
    my ($this) = @_;
    $this->{artifact_folders_prepared} = 0;
}

sub doRun
{
    my ($this) = @_;
    $this->writeTestPlan($this->testPlanFileName(), $this->getTestPlan());
    my @dllNames = $this->getDllNames();
    my @nunitArgs = $this->getNUnitArgs();
    my @monoArgs = $this->getMonoArgs();
    $this->{runnerExitCode} = $this->runNUnitProject(dlls => \@dllNames, directRunnerArgs => \@nunitArgs, monoArgs => \@monoArgs);
}

sub registerStaticArtifacts
{
    my ($this) = @_;
    my $stdOut = $this->nunitConsoleStdOutFileName();
    my $stdErr = $this->nunitConsoleStdErrFileName();
    $this->registerArtifacts($stdOut, $stdErr);
}

sub beforeRun
{
    my ($this) = @_;
    $this->writeTestPlan($this->testPlanFileName(), $this->getTestPlan());
    $this->setAssetPipelineEnvironment();
}

sub afterRun
{
    resetAssetPipelineEnvironment();
}

sub setAssetPipelineEnvironment
{
    my ($this) = @_;

    if ($this->getOptionValue('assetpipelinev2'))
    {
        $ENV{'UNITY_USE_ASSET_PIPELINE_V2'} = 1;
    }
    else
    {
        resetAssetPipelineEnvironment();
    }
}

sub resetAssetPipelineEnvironment
{
    delete $ENV{'UNITY_USE_ASSET_PIPELINE_V2'};
}

sub getMonoArgs
{
    return ('--debug');
}

sub getIncludes
{
    return ();
}

sub getExcludes
{
    return ();
}

1;
