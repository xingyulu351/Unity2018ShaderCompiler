package UnityRemote;

use strict;
use warnings;
use File::Basename;
use File::Path qw(mkpath rmtree);
use File::chdir;
use Carp qw (croak);
use lib File::Spec->rel2abs(dirname($0) . "/..");
use Tools qw (UCopy ProgressMessage);
use lib File::Spec->rel2abs(dirname($0) . "/../../External/Perl/lib");

sub Register
{
    Main::RegisterTarget("UnityRemote", { macbuildfunction => \&PackageUnityRemote });
}

sub PackageUnityRemote
{
    my ($root, $targetPath, $options) = @_;

    ProgressMessage("Fetching remote from SVN\n");
    my $remoteFolder = "$root/build/temp/unity_remote";
    rmtree($remoteFolder);
    system("rsync", "-av", "$root/Tools/iPhone/UnityRemote/", $remoteFolder);

    ProgressMessage("Archiving Unity Remote\n");
    local $CWD = "$remoteFolder";
    system("zip", '-9', '-T', '-r', "$targetPath/UnityRemote2.zip", ".") && croak("Could not archive Unity Remote.\n");
}

1;
