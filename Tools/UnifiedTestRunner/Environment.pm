package Environment;

use warnings;
use strict;

use Dirs;
use Cwd;
use Carp qw (croak);
use File::Spec::Functions qw (canonpath);

use BuildEngines::XBuild;

my $root = Dirs::getRoot();

sub new
{
    my ($pkg) = @_;
    my $instance = {
        systemCall => new SystemCall(),
        buildEngine => new BuildEngines::XBuild()
    };
    return bless($instance, $pkg);
}

sub setBuildEngine
{
    my ($this, $value) = @_;
    $this->{buildEngine} = $value;
}

sub getBuildEngine
{
    my ($this) = @_;
    return $this->{buildEngine};
}

sub setSystemCall
{
    my ($this, $value) = @_;
    $this->{systemCall} = $value;
}

sub getSystemCall
{
    my ($this) = @_;
    return $this->{systemCall};
}

sub getName
{
    croak("Environment should have have name.");
}

sub runDotNetProgram
{
    croak("Not implemented");
}

sub runNUnitProject
{
    croak("Not implemented");
}

sub getCgBatchTargetPath
{
    if ($^O eq 'MSWin32')
    {
        return canonpath("$root/build/WindowsEditor/Data/Tools/UnityShaderCompiler.exe");
    }
    elsif ($^O eq 'darwin')
    {
        return canonpath("$root/build/MacEditor/Unity.app/Contents/Tools/UnityShaderCompiler");
    }
    elsif ($^O eq 'linux')
    {
        return canonpath("$root/build/LinuxEditor/Data/Tools/UnityShaderCompiler");
    }
    else
    {
        croak('unsupported platform');
    }
}

sub getEdititorTargetPath
{
    if ($^O eq 'MSWin32')
    {
        return canonpath("$root/build/WindowsEditor/Unity.exe");
    }
    elsif ($^O eq 'darwin')
    {
        return canonpath("$root/build/MacEditor/Unity.app/Contents/MacOS/Unity");
    }
    elsif ($^O eq 'linux')
    {
        return canonpath("$root/build/LinuxEditor/Unity");
    }
    else
    {
        croak('unsupported platform');
    }
}

sub decorateArgumnetsForDotNetProgram
{
    croak("Not implemented");
}

1;
