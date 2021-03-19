package SuiteRunners::CGBatch;

use parent NativeBase;

use warnings;
use strict;

use Carp qw (croak);
use File::Basename qw (dirname);
use File::Spec::Functions qw (canonpath);

use LineTail;
use TargetResolver;
use threads::shared;
use SuiteResult;
use SuiteRunnerArtifactsNameBuilder;
use Option;

my $suiteOptions = [
    ['testresultsxml:s', 'Specifies an output xml file for test results.'],
    ['variation:s', 'External tool to run test suite under', { defaultValue => 32, allowedValues => ['32', '64'] }],

    #TODO overridie build  default value until it become, global default
    ['build-mode:s', 'Build mode', { hidden => 1, defaultValue => 'minimal' }],
    ['build-isolation:s', undef, { hidden => 1, defaultValue => 'enabled' }],
    ['tool:s', 'External tool to run test suite under', { allowedValues => ['tsan', 'insxpe'] }],
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
        description => 'Unit tests built-in into the editor UnityShaderCompiler',
        examples => [{ description => 'Run test for 64-bit shader compiler', command => '--variation=64' }]
    );
}

sub getName
{
    my ($this) = @_;
    return 'cgbatch';
}

sub getPlatform
{
    my ($this) = @_;
    return 'cgbatch';
}

sub doRun
{
    my ($this) = @_;
    my $shaderCompiler = $this->getCgBatchTargetPath();
    my $newdir = dirname($shaderCompiler);
    my @cmdArgs = $this->prepareCmdArgs();
    my $stdOut = $this->unityShaderCompilerStdOutFileName();
    my $stdErr = $this->unityShaderCompilerStdErrFileName();
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
    my $stdOut = $this->unityShaderCompilerStdOutFileName();
    my $stdErr = $this->unityShaderCompilerStdErrFileName();
    $this->registerArtifacts($stdOut, $stdErr);
}

sub prepareCmdArgs
{
    my ($this) = @_;
    my @cmdArgs = ();

    my $shaderCompiler = $this->getCgBatchTargetPath();
    push(@cmdArgs, $shaderCompiler);
    push(@cmdArgs, "-runNativeTests");

    my $filter = $this->getOptionValue('testfilter');
    if (defined($filter))
    {
        push(@cmdArgs, "$filter");
    }

    my $stdOut = $this->unityShaderCompilerStdOutFileName();

    $this->_addRunnerOptions(\@cmdArgs);
    push(@cmdArgs, "1>$stdOut");

    my $stdErr = $this->unityShaderCompilerStdErrFileName();
    push(@cmdArgs, "2>$stdErr");

    my $tool = $this->getTool();
    if ($tool)
    {
        @cmdArgs = $tool->decorate(@cmdArgs);
    }
    return @cmdArgs;
}

sub prepareArtifactsFolders
{
    my ($this) = @_;
    if ($this->{artifact_folders_prepared})
    {
        return;
    }
    $this->createArtifactsDirIfNotExistsOrCroak('UnityShaderCompiler');
    $this->createArtifactsDirIfNotExistsOrCroak('UnifiedTestRunner');
    $this->{artifact_folders_prepared} = 1;
}

sub stdOutFileName
{
    my ($this) = @_;
    return $this->unityShaderCompilerStdOutFileName();
}

sub stdErrFileName
{
    my ($this) = @_;
    return $this->unityShaderCompilerStdErrFileName();
}

sub unityShaderCompilerStdOutFileName
{
    my ($this) = @_;
    return $this->{artifactsNameBuilder}->getFullAtrifactFileName(
        'parent-type' => 'UnityShaderCompiler',
        type => 'stdout',
        extension => 'txt'
    );
}

sub unityShaderCompilerStdErrFileName
{
    my ($this) = @_;
    return $this->{artifactsNameBuilder}->getFullAtrifactFileName(
        'parent-type' => 'UnityShaderCompiler',
        type => 'stderr',
        extension => 'txt'
    );
}

sub getCgBatchTargetPath
{
    my ($this) = @_;
    my $root = Dirs::getRoot();
    if ($^O eq 'darwin')
    {
        return canonpath("$root/build/MacEditor/Unity.app/Contents/Tools/UnityShaderCompiler");
    }
    elsif ($^O eq 'MSWin32')
    {
        my $variation = $this->getOptionValue('variation', '32');
        if ($variation eq '64')
        {
            return canonpath("$root/build/WindowsEditor/Data/Tools64/UnityShaderCompiler.exe");
        }
        elsif ($variation eq '32')
        {
            return canonpath("$root/build/WindowsEditor/Data/Tools/UnityShaderCompiler.exe");
        }
        else
        {
            croak("unsupported variation $variation ");
        }
    }
    elsif ($^O eq 'linux')
    {
        return canonpath("$root/build/LinuxEditor/Data/Tools/UnityShaderCompiler");
    }
    else
    {
        croak("$^O is not supported");
    }
}

1;
