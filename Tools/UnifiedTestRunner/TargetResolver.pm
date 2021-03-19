package TargetResolver;

use warnings;
use strict;

use Dirs;
use Carp qw (croak);
use File::Spec::Functions qw (canonpath catfile);
use SystemCall;

my $root = Dirs::getRoot();

use lib Dirs::external_root();

sub resolve
{
    my ($platform, $os, $variation) = @_;
    $os = $^O if not defined($os);
    if ($platform eq 'editor')
    {
        return getEditorTargetPath($os);
    }

    if ($platform eq 'yamlmerge')
    {
        return getYAMLMergeTargetPath($os);
    }

    if ($platform eq 'standalone')
    {
        return getStandaloneTargetPath($os, $variation);
    }

    croak("Platform '$platform' is not supported by UTR");
}

sub getEditorTargetPath
{
    my ($os) = @_;

    if ($os eq 'MSWin32')
    {
        my $executable = getExecutable('WindowsEditor');
        return canonpath("$root/build/WindowsEditor/$executable");
    }

    if ($os eq 'darwin')
    {
        my $appFolder = getExecutable('MacEditor');
        return canonpath("$root/build/MacEditor/$appFolder/Contents/MacOS/Unity");
    }

    if ($os eq 'linux')
    {
        my $executable = getExecutable('LinuxEditor');
        return canonpath("$root/build/LinuxEditor/$executable");
    }

    croak("Operating system '$os' is not supported by UTR for this platform");
}

sub getYAMLMergeTargetPath
{
    my ($os) = @_;
    if ($os eq 'MSWin32' or $os eq 'darwin' or $os eq 'linux')
    {
        return canonpath("$root/Editor/Tools/UnityYAMLMerge/TestData/runalltests.pl");
    }
    croak('Unsupported platform');
}

sub getCgBatchTargetPath
{
    my ($os, $variation) = @_;
    if ($os eq 'MSWin32')
    {
        if (defined($variation) and $variation eq 'win64')
        {
            return canonpath("$root/build/WindowsEditor/Data/Tools64/UnityShaderCompiler.exe");
        }
        return canonpath("$root/build/WindowsEditor/Data/Tools/UnityShaderCompiler.exe");
    }
    if ($os eq 'darwin')
    {
        return canonpath("$root/build/MacEditor/Unity.app/Contents/Tools/UnityShaderCompiler");
    }
    if ($os eq 'linux')
    {
        return canonpath("$root/build/LinuxEditor/Data/Tools/UnityShaderCompiler");
    }

    croak("Can not resolve cgbatch executable for $os, $variation");
}

sub getStandaloneTargetPath
{
    my ($os, $variation) = @_;
    if ($os eq 'MSWin32')
    {
        if (not(defined($variation)))
        {
            $variation = "win64_development_mono";
        }
        return canonpath("$root/build/WindowsStandaloneSupport/Variations/$variation/WindowsPlayer.exe");
    }

    if ($os eq 'darwin')
    {
        if (not(defined($variation)))
        {
            $variation = 'macosx64_development_mono';
        }
        return canonpath("$root/build/MacStandaloneSupport/Variations/$variation/UnityPlayer.app/Contents/MacOS/UnityPlayer");
    }
    if ($os eq 'linux')
    {
        return canonpath("$root/build/LinuxStandaloneSupport/Variations/$variation/LinuxPlayer");
    }
    croak('Unsupported platform');
}

sub getExecutable
{
    my ($module) = @_;
    my $systemCall = new SystemCall();
    my $resolver = catfile(Dirs::UTR_root(), 'resolve_build_target.pl');

    # executable resolving requires loading some external modules
    # it is safer to do this in another process, since UTR could have some
    # of the modules already loaded and it could conflict with modules
    # other module wants to load.
    my ($exitCode, $executable) = $systemCall->executeAndGetStdOut('perl', $resolver, $module);
    if ($exitCode != 0)
    {
        croak("failed to resolve executable for module '$module'");
    }
    return $executable;
}

1;
