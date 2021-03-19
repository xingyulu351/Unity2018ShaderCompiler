package TestHelpers::Runners;

use warnings;
use strict;

use Test::More;

use File::Spec::Functions qw (canonpath);
use File::Temp qw (tempdir);
use Carp qw (croak);
use Options;

use RunContext;
use GlobalOptions;
use ArtifactsNameBuilder;
use ProgressPresenters::Null;
use ArtifactsMap;

use Dirs;
use lib Dirs::external_root();
use Switch;
use Test::MockObject;
use Test::MockObject::Extends;
use TestHelpers::Stub;

use Exporter 'import';

our @EXPORT_OK =
    qw [runAndGrabExecuteInput listAndGrabExecuteInput runAndGrabBuildEngineInput makeDefaultSuiteRunnerArtifactNameBuilderStub list buildPlayerAndGrabExecuteInput makeRunnerMock];

sub runAndGrabExecuteInput
{
    my ($runner, @runnerArgs) = @_;
    my ($systemArgs) = run($runner, @runnerArgs);
    return @$systemArgs;
}

sub buildPlayerAndGrabExecuteInput
{
    my ($runner, @runnerArgs) = @_;
    my ($systemArgs, $buildEngineArgs) = buildPlayer($runner, @runnerArgs);
    return @$systemArgs;
}

sub runAndGrabBuildEngineInput
{
    my ($runner, @runnerArgs) = @_;
    my ($systemArgs, $buildEngineArgs) = run($runner, @runnerArgs);
    return @$buildEngineArgs;
}

sub listAndGrabExecuteInput
{
    my ($runner, @runnerArgs) = @_;
    my ($systemArgs) = list($runner, @runnerArgs);
    return @$systemArgs;
}

sub list
{
    my ($runner, @args) = @_;
    $runner = makeRunnerMock($runner, @args);
    my @listOutput = $runner->list(@args);
    my @systemArgs = getSystemCallArgs($runner);
    my @buildEngineArgs = getBuildSystemArgs($runner);
    return (\@systemArgs, \@buildEngineArgs, \@listOutput);
}

sub run
{
    my ($runner, @args) = @_;
    $runner = makeRunnerMock($runner, @args);
    $runner->run(@args);
    my @systemArgs = getSystemCallArgs($runner);
    my @buildEngineArgs = getBuildSystemArgs($runner);
    return (\@systemArgs, \@buildEngineArgs);
}

sub buildPlayer
{
    my ($runner, @args) = @_;
    $runner = makeRunnerMock($runner, @args);
    $runner->buildPlayer(@args);
    my @systemArgs = getSystemCallArgs($runner);
    my @buildEngineArgs = getBuildSystemArgs($runner);
    return (\@systemArgs, \@buildEngineArgs);
}

sub makeRunnerMock
{
    my ($runner, @args) = @_;

    my @systemArgs = ();
    my @filesToWatch = ();
    my $systemMock = createSystemCallMock();

    $systemMock->mock('execute', sub { shift; @systemArgs = @_; });
    $systemMock->mock(
        'executeAndWatchFiles',
        sub
        {
            shift;
            my %p = @_;
            @systemArgs = @{ $p{command} };
            if ($p{filesToWatch})
            {
                @filesToWatch = @{ $p{filesToWatch} };
            }
            return 0;
        }
    );
    $systemMock->mock('getArgs', sub { return @systemArgs; });
    $systemMock->mock('getFilesToWatch', sub { return @filesToWatch; });

    my @buildEngineArgs = ();
    my $buildEngine = createBuildEngineMock();

    $buildEngine->mock('build', sub { shift; @buildEngineArgs = @_; });
    $buildEngine->mock('getArgs', sub { return @buildEngineArgs; });

    my $environment = createEnvironment($systemMock, $buildEngine);
    my $progressTracker = new ProgressPresenters::Null();
    my $servicesMock = createServicesMock();
    my $artifactsMock = createArtifactsMock();

    $runner->setEnvironment($environment);
    $runner->setProgressPresenter($progressTracker);
    $runner->setArtifacts($artifactsMock);

    my @runnerOptions = @{ $runner->getOptions() };
    my @allOptions = (@runnerOptions, GlobalOptions::getOptions());

    my $options = new Options(options => \@allOptions);

    $options->parse(@args);

    my $runContext = RunContext::new($options);
    $runContext = new Test::MockObject::Extends($runContext);
    $runContext->mock(getArtifactsPath => sub { return 'temp_path'; });
    $runner->setRunContext($runContext);
    $runner = mockSuiteRunnerExternalCalls($runner);
    return $runner;
}

sub getSystemCallArgs
{
    my ($runner) = @_;
    my $environment = $runner->getEnvironment();
    my $systemCall = $environment->getSystemCall();
    return $systemCall->getArgs();
}

sub getBuildSystemArgs
{
    my ($runner) = @_;
    my $environment = $runner->getEnvironment();
    my $buildEngine = $environment->getBuildEngine();
    return $buildEngine->getArgs();
}

sub mockSuiteRunnerExternalCalls
{
    my ($runner) = @_;
    $runner = Test::MockObject::Extends->new($runner);
    my $dummyReadContentContent = "";
    $runner->mock(
        '_openFileForRead',
        sub
        {
            open(my $fh, '<', \$dummyReadContentContent);
            return $fh;
        }
    );
    my $dummyWriteContentContent = "";
    $runner->mock(
        '_openFileForWrite',
        sub
        {
            open(my $fh, '>', \$dummyWriteContentContent);
            return $fh;
        }
    );

    $runner->mock('parseTestResultsXml', sub { my %res; $res{testResults} = []; return %res; });
    $runner->mock('createArtifactsRootFolder', sub { });
    $runner->mock('registerArtifactsToWatch', sub { });
    $runner->mock('cleanCSProject', sub { });
    $runner->mock('prepareArtifactsFolders', sub { });
    $runner->mock('buildCSProject2', sub { return undef });

    return $runner;
}

sub createSystemCallMock
{
    my $systemMock = Test::MockObject->new();
    $systemMock->mock('execute', sub { });
    return $systemMock;
}

sub createBuildEngineMock
{
    my $buildEngine = Test::MockObject::Extends->new();
    $buildEngine->mock('build', sub { });
    $buildEngine->mock('logfile', sub { });
    return $buildEngine;
}

sub createServicesMock
{
    my $servicesMock = Test::MockObject->new();
    my $lineTailMock = Test::MockObject->new();
    $lineTailMock->mock('start', sub { });
    $lineTailMock->mock('stop', sub { });
    $servicesMock->mock('getLineTail', sub { return $lineTailMock; });
    return $servicesMock;
}

sub createEnvironment
{
    my ($systemMock, $buildEngine) = @_;
    my $environment = Environments::Mono->new();
    $environment->setSystemCall($systemMock);
    $environment->setBuildEngine($buildEngine);
    return $environment;
}

sub createArtifactsMock
{
    my $artifactsMock = Test::MockObject->new();
    my @artifacts = ();
    $artifactsMock->mock('register', sub { my ($this, $suite, $artifact) = @_; push(@artifacts, $artifact) });
    $artifactsMock->mock('get', sub { return @artifacts; });
    return $artifactsMock;
}

sub makeDefaultSuiteRunnerArtifactNameBuilderStub
{
    my $devnull = File::Spec->devnull();

    my $nameBuilderStub = new TestHelpers::Stub();
    $nameBuilderStub->specifyExpectation(
        method => 'getFullFolderPathForArtifactType',
        result => '/some_temporary_folder'
    );

    $nameBuilderStub->specifyExpectation(
        method => 'getFullAtrifactFileName',
        result => $devnull
    );

    $nameBuilderStub->specifyExpectation(
        method => 'getSuiteRunRoot',
        result => '/some_temporary_folder'
    );

    return $nameBuilderStub;
}

sub getFileNames
{
    my %params = @_;

    my $root = Dirs::getRoot();
    my $suiteName = $params{suite};
    my $artifactsNameBuilder = $params{name_builder};
    my $utrRoot = Dirs::UTR_root();
    my @requestedArtifactNames = @{ $params{artifacts} };

    my (@results);
    for my $aid (@requestedArtifactNames)
    {
        switch (lc($aid))
        {
            case 'mono' { push(@results, canonpath("$utrRoot/mono")) }
            case 'root' { push(@results, $root) }
            case 'lister' { push(@results, canonpath("$root/Tests/Unity.TestLister/bin/Debug/Unity.TestLister.exe")) }
            else { croak("$aid is unknown artifact name"); }
        }
    }

    return @results;
}

1;
