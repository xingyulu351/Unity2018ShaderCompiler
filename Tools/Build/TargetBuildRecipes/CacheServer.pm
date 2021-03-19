package CacheServer;

use strict;
use warnings;
use File::Basename;
use File::Path qw (mkpath rmtree);
use File::Spec;
use File::chdir;
use lib File::Spec->rel2abs(dirname($0) . "/..");
use Carp qw (croak carp);
use Tools qw (UCopy ProgressMessage);
use lib File::Spec->rel2abs(dirname($0) . "/../../External/Perl/lib");
use lib ('.', "./Tools/Build");
use PrepareWorkingCopy qw(UnzipToDirectory GetSmartFilter);

## Get build options
use BuildConfig;
our $unityVersion = $BuildConfig::unityVersion;

sub Register
{
    Main::RegisterTarget("CacheServer", { macbuildfunction => \&CacheServer });
    Main::RegisterTarget("CacheServer", { linuxbuildfunction => \&CacheServer });
    Main::RegisterTarget("CacheServer", { windowsbuildfunction => \&CacheServer });
}

sub CacheServer
{
    my ($root, $targetPath, $options) = @_;

    rmtree("$targetPath/CacheServer");
    mkpath("$targetPath/CacheServer");
    mkpath("$targetPath/CacheServer/nodejs");

    ProgressMessage("Unpacking nodejs for Cache Server\n");

    my @arrayOfZips = qw(External/nodejs/builds_win.zip External/nodejs/builds_macosx.zip External/nodejs/builds_linux.zip);
    foreach my $zip (@arrayOfZips)
    {
        my @smart = GetSmartFilter($zip);
        UnzipToDirectory($root, $zip, "$targetPath/CacheServer/nodejs", 0, @smart);
    }

    ProgressMessage("Packaging cache server\n");

    UCopy("$root/External/CacheServer/CacheServer.js", "$targetPath/CacheServer");
    UCopy("$root/External/CacheServer/LegacyCacheServer.js", "$targetPath/CacheServer");
    UCopy("$root/External/CacheServer/main.js", "$targetPath/CacheServer");
    UCopy("$root/External/CacheServer/RunLinux.sh", "$targetPath/CacheServer");
    UCopy("$root/External/CacheServer/RunOSX.command", "$targetPath/CacheServer");
    UCopy("$root/External/CacheServer/RunWin.cmd", "$targetPath/CacheServer");

    if ($^O eq 'linux')
    {

        # Build ubuntu package
        chdir("$targetPath/CacheServer");
        UCopy("RunLinux.sh", "CacheServer");
        for my $file ('control', 'postinstall', 'prerm', 'unity3d-cacheserver')
        {
            UCopy("$root/External/CacheServer/ubuntu/$file", "$targetPath/CacheServer");
        }
        system('sed', '-i.orig', "s/%version%/$unityVersion/", "$targetPath/CacheServer/control");
        system('equivs-build', 'control');
        for my $file ('control', 'control.orig', 'postinstall', 'prerm', 'unity3d-cacheserver', 'CacheServer')
        {
            rmtree("$targetPath/CacheServer/$file");
        }
    }

    chdir($targetPath);
    system('zip', '-9', '-T', '-r', 'CacheServer.zip', 'CacheServer') && croak("Could not archive cache server.\n");
}

1;
