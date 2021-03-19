package SuiteRunners::Collab;

use File::Copy;

use parent SuiteRunner;
use parent FolderDeleter;
use parent Logger;

use warnings;
use strict;

use TargetResolver;
use Switch;
use UnityTestProtocol::Utils;
use TestResult;
use FileUtils qw (openForReadOrCroak closeOrCroak);
use Option;

use Env qw (CTFUSERNAME CTFPASSWORD);
use Carp qw( croak );

my $root = Dirs::getRoot();

my $suiteOptions = [
    ['ctf:s', "CTF test tags (comma separated list)"],
    ['username:s', "CTF username"],
    ['password:s', "CTF passsword"],
    ['cloudOrganization:s', "CTF organization"],
    ['cloudEnvironment:s', "CTF environment"],
    ['extraArgs:s', "CTF extra args passed to Unity"],
    ['logFile:s', "CTF specific logFile"],
];

sub usage
{
    my ($this) = @_;
    return (
        description => "Tests basic Collab features like detecting changes, publishing, etc.",
        examples => [
            { description => 'Run all tests', command => '' },
            { description => 'Run QA tests', command => '--ctf=qa' },
            { description => 'Run QA & Dev tests', command => '--ctf=qa,dev' },
            { description => 'Run specific test (ie "Restore: ignored files")', command => '--ctf=name:Restore__ignored_files' },
            { description => 'Run specific fixture tests', command => '--ctf=fixture:ScriptTest' },
            { description => 'Run related bug 999999 test', command => '--ctf=case:999999' },
            { description => 'Run all ignored tests', command => '--ctf=IGNORED' },
            { description => 'Run all tests mark as RunInIsolation', command => '--ctf=RUNINISOLATION' },
            { description => 'Run all tests that use specific operation OpName (ie Revert)', command => '--ctf=op:Revert' },
            { description => 'Pass extra arguments to Unity', command => '--ctf=dev --extraArgs="-debugAllocator"' },
        ]
    );
}

sub getName
{
    return 'collab';
}

sub getPlatform
{
    return 'editor';
}

sub prepareArtifactsFolders
{
    my ($this) = @_;
    if ($this->{artifact_folders_prepared})
    {
        return;
    }
    $this->createArtifactsDirIfNotExistsOrCroak('Unity');
    $this->{artifact_folders_prepared} = 1;
}

sub new
{
    my ($pkg) = @_;
    my $this = $pkg->SUPER::new(suiteOptions => $suiteOptions);
    $this->{artifactsNameBuilder} = new SuiteRunnerArtifactsNameBuilder($this);

    $this->{totalTests} = 0;
    $this->{currentTest} = 0;

    return $this;
}

sub startNewIteration
{
    my ($this) = @_;
    $this->{artifact_folders_prepared} = 0;
}

sub registerStaticArtifacts
{
    my ($this) = @_;
    my $stdOut = $this->getCollabTestLogFile();
    $this->registerArtifacts($stdOut);
}

sub doRun
{
    my ($this) = @_;
    my $logFile = $this->getCollabTestLogFile();
    my @args;

    my $username = $this->getOptionValue('username');
    if (defined($username))
    {
        push(@args, "-username $username");
    }
    else
    {
        if (defined($CTFUSERNAME))
        {
            push(@args, "-username $CTFUSERNAME");
        }
        else
        {
            croak("No CTF username defined in env variable CTFUSERNAME");
        }
    }

    my $password = $this->getOptionValue('password');
    if (defined($password))
    {
        push(@args, "-password $password");
    }
    else
    {
        if (defined($CTFPASSWORD))
        {
            push(@args, "-password $CTFPASSWORD");
        }
        else
        {
            croak("No CTF password defined in env variable CTFPASSWORD");
        }
    }

    my $environment = $this->getOptionValue('cloudEnvironment');
    if (defined($environment))
    {
        push(@args, "-cloudEnvironment $environment");
    }
    else
    {
        push(@args, "-cloudEnvironment staging");
    }

    my $organization = $this->getOptionValue('cloudOrganization');
    if (defined($organization))
    {
        push(@args, "-cloudOrganization $organization");
    }
    else
    {
        push(@args, "-cloudOrganization collabtest");
    }

    push(@args, "-enableServices collab");
    push(@args, "-enableCollabEventLogs");
    push(@args, "-projectPath $root/Tests/CollabTests/CollabTestProject_uproj");
    push(@args, "-batchmode");
    push(@args, "-executeMethod UnityEditor.Collaboration.Testing.Framework.Main.RunTestsWhenCollabIsReady");
    push(@args, "-logFile $logFile");

    my $tags = $this->getOptionValue('ctf');
    if (defined($tags))
    {
        push(@args, "-ctf $tags");
    }

    my $extraArgs = $this->getOptionValue('extraArgs');
    if (defined($extraArgs))
    {
        push(@args, $extraArgs);
    }

    my $os = $^O;
    if ($os eq "MSWin32")
    {
        $os = "windows";
    }
    elsif ($os eq "darwin")
    {
        $os = "macos";
    }

    Logger::minimal("Erase Browser cache");
    my $deleter = new FolderDeleter();
    if ($os eq "windows")
    {
        $deleter->delete("$ENV{'APPDATA'}/../LocalLow/Unity/Browser");
    }
    elsif ($os eq "macos")
    {
        $deleter->delete("$ENV{'HOME'}/Library/Unity/Browser");
    }

    Logger::minimal("Erase Library and Temp folders");
    $deleter->delete("$root/Tests/CollabTests/CollabTestProject_uproj/Library");
    $deleter->delete("$root/Tests/CollabTests/CollabTestProject_uproj/Temp");

    Logger::minimal("Copy ProjectSettings.asset backup");
    unlink("$root/Tests/CollabTests/CollabTestProject_uproj/ProjectSettings/ProjectSettings.asset");
    copy(
        "$root/Tests/CollabTests/CollabTestProject_uproj/Backup/ProjectSettings.asset",
        "$root/Tests/CollabTests/CollabTestProject_uproj/ProjectSettings/ProjectSettings.asset"
    );

    my $editor = TargetResolver::resolve('editor');
    $this->executeAndWatchFiles(
        command => [$editor, @args],
        filesToWatch => [{ file => $logFile, callback => \&trackProgress, callbackArg => $this }]
    );
}

sub trackProgress
{
    my ($logLine, $this) = @_;
    my $msg = UnityTestProtocol::Utils::getMessage($logLine);

    if (not defined($msg))
    {
        return;
    }

    switch ($msg->{type})
    {
        case 'TestRunStart' { $this->processTestRunStartedMessage($msg); }
        case 'TestStart' { $this->processTestStartedMessage($msg); }
        case 'TestFinish' { $this->processTestFinishedMessage($msg); }
        case 'TestLog' { $this->processTestLoggedMessage($msg); }
        case 'TestLogError' { $this->processTestLoggedMessage($msg); }
    }
}

sub processTestFinishedMessage
{
    my ($this, $msg) = @_;
    my $progressPresenter = $this->getProgressPresenter();

    my $stateString = TestResult::stateToString($msg->{state});

    $progressPresenter->stepDone(
        progress => { current => $this->{currentTest}, total => $this->{totalTests} },
        progressDetails => "$msg->{name} $stateString"
    );
}

sub processTestStartedMessage
{
    my ($this, $msg) = @_;

    ++$this->{currentTest};

    my $progressPresenter = $this->getProgressPresenter();
    $progressPresenter->stepStarted(
        progress => { current => $this->{currentTest}, total => $this->{totalTests} },
        progressDetails => $msg->{name}
    );
}

sub processTestRunStartedMessage
{
    my ($this, $msg) = @_;

    if (not defined $msg->{testsCount})
    {
        return;
    }

    $this->{totalTests} = $msg->{testsCount};
}

sub processTestLoggedMessage
{
    my ($this, $msg) = @_;
    Logger::minimal("$msg->{message}");
}

sub getCollabTestLogFile
{
    my ($this) = @_;

    my $log = $this->getOptionValue('logFile');
    if (defined($log))
    {
        return $log;
    }

    return $this->{artifactsNameBuilder}->getFullAtrifactFileName(
        'parent-type' => 'Unity',
        type => 'stdout',
        extension => 'txt'
    );
}

sub processResults
{
    my ($this) = @_;

    my $resultConsumer = $this->getResultConsumer();
    if (not defined($resultConsumer))
    {
        return;
    }

    my $logFile = $this->getCollabTestLogFile();
    my $fh = openForReadOrCroak($logFile);
    my $suiteResult = new SuiteResult(
        name => $this->getName(),
        platform => $this->getPlatform()
    );

    while (<$fh>)
    {
        my $msg = UnityTestProtocol::Utils::getMessage($_);

        if (defined($msg) and $msg->{type} eq 'TestFinish')
        {
            my $testResult = new TestResult();
            $testResult->setTest($msg->{name});
            $testResult->setState($msg->{state});
            $testResult->setMessage($msg->{message});
            $testResult->setTime($msg->{duration});

            $suiteResult->addTestResult($testResult);
        }
    }
    closeOrCroak($fh, $logFile);

    $this->sendResultToConsumer($suiteResult);
}

1;
