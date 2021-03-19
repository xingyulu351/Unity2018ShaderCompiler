package VuforiaSupport;

use Cwd;
use File::Basename qw(dirname basename fileparse);
use File::Path qw (rmtree);
use lib File::Spec->rel2abs(dirname(__FILE__));
use File::Basename;
use Tools qw (Jam extractArtifact UCopy DeleteRecursive);
use Tools qw (PackageAndSendResultsToBuildServer);

# use strict;
use warnings;

sub Register()
{
    Main::RegisterTarget(
        "VuforiaSupport",
        {
            windowsbuildfunction => \&BuildVuforia,
        }
    );

    Main::RegisterTarget(
        "VuforiaSupport",
        {
            macbuildfunction => \&BuildVuforia,
        }
    );
}

sub BuildVuforia()
{
    my ($root, $targetPath, $options) = @_;

    rmtree("$root/artifacts/XR/vuforia");

    extractArtifact("$root/External/XR/vuforia/builds.zip", "$root/External/XR/vuforia/builds");
    if ($^O eq "MSWin32")
    {
        Jam($root, "VuforiaSupport", "", 'win64', 1, $options->{jamArgs});
    }
    else
    {
        Jam($root, "VuforiaSupport", "", 'macosx64', 1, $options->{jamArgs});
    }

    rmtree("$root/build/VuforiaSupport");
    UCopy("$root/External/XR/vuforia/builds/install_with_editor/", "$root/build/VuforiaSupport/");
    UCopy("$root/artifacts/XR/vuforia/", "$root/build/VuforiaSupport/");

    if ($^O eq "MSWin32")
    {
        rmtree("$root/build/WindowsEditor/Data/PlaybackEngines/VuforiaSupport");
        UCopy("$root/External/XR/vuforia/builds/install_with_editor/", "$root/build/WindowsEditor/Data/PlaybackEngines/VuforiaSupport/");
        UCopy("$root/artifacts/XR/vuforia/", "$root/build/WindowsEditor/Data/PlaybackEngines/VuforiaSupport/");
    }
    else
    {
        rmtree("$root/build/MacEditor/PlaybackEngines/VuforiaSupport");
        UCopy("$root/External/XR/vuforia/builds/install_with_editor/", "$root/build/MacEditor/PlaybackEngines/VuforiaSupport/");
        UCopy("$root/artifacts/XR/vuforia/", "$root/build/MacEditor/PlaybackEngines/VuforiaSupport/");
    }

    PackageAndSendResultsToBuildServer("$root/build/VuforiaSupport", "VuforiaSupport");
}

1;
