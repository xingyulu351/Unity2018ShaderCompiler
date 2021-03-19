package SuiteRunners::YAMLMerge;

use parent NativeBase;

use warnings;
use strict;

use File::Basename qw (dirname);
use LineTail;
use TargetResolver;
use threads::shared;
use SuiteResult;
use SuiteRunnerArtifactsNameBuilder;
use lib Dirs::external_root();
use File::pushd;
use Option;

my $suiteOptions = [
    ['testresultsxml:s', 'Specifies an output xml file for test results.'],
    ['mode:s', "Limit tests to either 'native' or 'integration'. Native tests are much faster to run.", { allowedValues => ['native', 'integration'] }],
];

sub new
{
    my ($pkg) = @_;
    return $pkg->SUPER::new(suiteOptions => $suiteOptions);
}

sub usage
{
    my ($this) = @_;
    return (
        description => 'Unity tests built-in into the UnityYAMLMerge test',
        examples => [{ description => 'Run native YAMLMerge tests', command => '--mode=native' }]
    );
}

sub getName
{
    my ($this) = @_;
    return 'YAMLMerge';
}

sub getPlatform
{
    my ($this) = @_;
    return 'YAMLMerge';
}

sub doRun
{
    my ($this) = @_;
    my $yamlmerge = TargetResolver::resolve('yamlmerge');
    my $newdir = pushd(dirname($yamlmerge));

    my @cmdArgs = $this->prepareCmdArgs();
    my $stdOut = $this->stdOutFileName();
    my $stdErr = $this->stdErrFileName();
    $this->{runnerExitCode} = $this->executeAndWatchFiles(
        command => \@cmdArgs,
        filesToWatch => [
            { file => $stdOut, callback => \&NativeBase::printProgressMessage, callbackArg => $this },
            { file => $stdOut, callback => \&Plugin::printLogMessage, callbackArg => $this },
            { file => $stdErr, callback => \&Plugin::printLogErrorMessage, callbackArg => $this },
        ]
    );
}

sub registerStaticArtifacts
{
    my ($this) = @_;
    my $stdOut = $this->stdOutFileName();
    my $stdErr = $this->unitYAMLMergeStdErrFileName();
    $this->registerArtifacts($stdOut, $stdErr);
}

sub prepareCmdArgs
{
    my ($this) = @_;
    my $mode = $this->getOptionValue('mode');

    my @cmdArgs = ("perl");

    my $yamlmerge = TargetResolver::resolve('yamlmerge');
    push(@cmdArgs, $yamlmerge);

    if (defined($mode))
    {
        push(@cmdArgs, $mode);
    }

    my $stdOut = $this->unitYAMLMergeStdOutFileName();
    push(@cmdArgs, "1>$stdOut");

    my $stdErr = $this->unitYAMLMergeStdErrFileName();
    push(@cmdArgs, "2>$stdErr");

    return @cmdArgs;
}

sub prepareArtifactsFolders
{
    my ($this) = @_;
    if ($this->{artifact_folders_prepared})
    {
        return;
    }
    $this->createArtifactsDirIfNotExistsOrCroak('UnityYAMLMerge');
    $this->createArtifactsDirIfNotExistsOrCroak('UnifiedTestRunner');
    $this->{artifact_folders_prepared} = 1;
}

sub stdOutFileName
{
    my ($this) = @_;
    return $this->unitYAMLMergeStdOutFileName();
}

sub stdErrFileName
{
    my ($this) = @_;
    return $this->unitYAMLMergeStdErrFileName();
}

sub unitYAMLMergeStdOutFileName
{
    my ($this) = @_;
    return $this->{artifactsNameBuilder}->getFullAtrifactFileName(
        'parent-type' => 'UnityYAMLMerge',
        type => 'stdout',
        extension => 'txt'
    );
}

sub unitYAMLMergeStdErrFileName
{
    my ($this) = @_;
    return $this->{artifactsNameBuilder}->getFullAtrifactFileName(
        'parent-type' => 'UnityYAMLMerge',
        type => 'stderr',
        extension => 'txt'
    );
}

1;
