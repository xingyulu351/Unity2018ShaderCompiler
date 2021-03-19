package SuiteRunners::UnWrapCL;

use parent NativeBase;

use warnings;
use strict;

use File::pushd;
use File::Basename qw (dirname);
use File::Spec::Functions qw (canonpath);
use Carp qw (croak);

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
        description => 'Unit tests built-in into the UnWrapCL tool',
        examples => [{ description => 'Run all tests', command => '' },]
    );
}

sub getName
{
    return 'UnWrapCL';
}

sub getPlatform
{
    return getName();
}

sub doRun
{
    my ($this) = @_;
    my @cmdArgs;
    my $unWrapCL = _getUnwrapCLTargetPath();
    push(@cmdArgs, $unWrapCL);
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
    my $newDirRef = $this->_pushd(dirname($unWrapCL));
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
    $this->createArtifactsDirIfNotExistsOrCroak('UnWrapCL');
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
        'parent-type' => 'UnWrapCL',
        type => 'stdout',
        extension => 'txt'
    );
}

sub stdErrFileName
{
    my ($this) = @_;
    return $this->{artifactsNameBuilder}->getFullAtrifactFileName(
        'parent-type' => 'UnWrapCL',
        type => 'stderr',
        extension => 'txt'
    );
}

sub _getUnwrapCLTargetPath
{
    my ($this) = @_;
    my $root = Dirs::getRoot();
    if ($^O eq 'darwin')
    {
        return canonpath("$root/build/MacEditor/Unity.app/Contents/Tools/UnwrapCL");
    }
    elsif ($^O eq 'MSWin32')
    {
        return canonpath("$root/build/WindowsEditor/Data/Tools/UnwrapCL.exe");
    }
    elsif ($^O eq 'linux')
    {
        return canonpath("$root/build/LinuxEditor/Data/Tools/UnwrapCL");
    }
    else
    {
        croak("$^O is not supported");
    }
}

sub _pushd
{
    my ($this, $dir) = @_;
    return pushd($dir);
}

1;
