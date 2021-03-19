package SuiteRunners::DocCombiner;

use parent NativeBase;

use warnings;
use strict;

use File::Spec::Functions qw (catdir canonpath);
use Carp qw (croak);
use SDKInstaller;

my $suiteOptions = [

    #TODO overridie build  default value until it become, global default
    ['build-mode:s', 'Build mode', { hidden => 1, defaultValue => 'minimal' }],
    ['build-isolation:s', undef, { hidden => 1, defaultValue => 'enabled' }],
    ['testresultsxml:s', 'Specifies an output xml file for test results.'],
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
        description => 'Unit tests built-in into the DocCombiner tool',
        examples => [{ description => 'Run all tests', command => '' },]
    );
}

sub getName
{
    return 'DocCombiner';
}

sub getPlatform
{
    return getName();
}

sub buildDependencies
{
    my ($this) = @_;

    if ($this->{dependencies_built})
    {
        return;
    }

    _installSDK();

    $this->SUPER::buildDependencies();
    $this->jam('DocCombiner');
    $this->{dependencies_built} = 1;
}

sub _installSDK
{
    my $shouldInstallSDK = (
        $^O eq 'linux' && (_isBuildAgent()
            || exists $ENV{'UNITY_USE_LINUX_SDK'})
    );

    if (not $shouldInstallSDK)
    {
        return;
    }

    my $artifactsFolder = catdir(Dirs::getRoot(), 'artifacts');
    SDKInstaller::installSDK('linux-sdk', '20180406', $artifactsFolder);
}

sub doRun
{
    my ($this) = @_;
    my @cmdArgs;
    my $docCombiner = _getDocCombinerTargetPath();
    push(@cmdArgs, $docCombiner);
    push(@cmdArgs, '-runNativeTests');

    my $filter = $this->getOptionValue('testfilter');
    if (defined($filter))
    {
        push(@cmdArgs, "$filter");
    }

    my $stdOut = $this->stdOutFileName();
    push(@cmdArgs, "1>$stdOut");

    my $stdErr = $this->stdErrFileName();
    push(@cmdArgs, "2>$stdErr");
    $this->{runnerExitCode} = $this->executeAndWatchFiles(
        command => \@cmdArgs,
        filesToWatch => [
            { file => $stdOut, callback => \&NativeBase::printProgressMessage, callbackArg => $this },
            { file => $stdOut, callback => \&Plugin::printLogMessage, callbackArg => $this },
            { file => $stdErr, callback => \&Plugin::printLogErrorMessage, callbackArg => $this },
        ]
    );
}

sub prepareArtifactsFolders
{
    my ($this) = @_;
    if ($this->{artifact_folders_prepared})
    {
        return;
    }
    $this->createArtifactsDirIfNotExistsOrCroak('DocCombiner');
    $this->createArtifactsDirIfNotExistsOrCroak('UnifiedTestRunner');
    $this->{artifact_folders_prepared} = 1;
}

sub registerStaticArtifacts
{
    my ($this) = @_;
    my $stdOut = $this->stdOutFileName();
    my $stdErr = $this->stdErrFileName();
    $this->registerArtifacts($stdOut, $stdErr);
}

sub stdOutFileName
{
    my ($this) = @_;
    return $this->{artifactsNameBuilder}->getFullAtrifactFileName(
        'parent-type' => 'DocCombiner',
        type => 'stdout',
        extension => 'txt'
    );
}

sub stdErrFileName
{
    my ($this) = @_;
    return $this->{artifactsNameBuilder}->getFullAtrifactFileName(
        'parent-type' => 'DocCombiner',
        type => 'stderr',
        extension => 'txt'
    );
}

sub _getDocCombinerTargetPath
{
    my ($this) = @_;
    my $root = Dirs::getRoot();
    if ($^O eq 'darwin')
    {
        return canonpath("$root/build/MacDocCombiner/DocCombiner");
    }
    elsif ($^O eq 'MSWin32')
    {
        return canonpath("$root/build/WindowsDocCombiner/DocCombiner.exe");
    }
    elsif ($^O eq 'linux')
    {
        return canonpath("$root/build/LinuxDocCombiner/DocCombiner");
    }
    else
    {
        croak("$^O is not supported");
    }
}

sub _isBuildAgent
{
    my $thisIsABuildMachine = $ENV{'UNITY_THISISABUILDMACHINE'};
    if (defined($thisIsABuildMachine) and $thisIsABuildMachine eq 1)
    {
        return 1;
    }
    return 0;
}

1;
