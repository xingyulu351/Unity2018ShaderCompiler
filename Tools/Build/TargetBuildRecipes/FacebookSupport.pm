package FacebookSupport;

use strict;
use warnings;
use File::Basename qw (dirname basename fileparse);
use File::Copy;
use File::Spec;
use File::Path;
use File::Glob;
use File::Find;
use Carp qw (croak carp);
use File::chdir;
use Tools qw (Jam JamRaw UCopy ProgressMessage PackageAndSendResultsToBuildServer);
use Tools qw (NewXBuild);
use WinTools qw (SignFile);

sub Register
{
    Main::RegisterTarget(
        "Facebook",
        {
            windowsbuildfunction => \&BuildFacebookSupport,
            options => {
                "codegen" => ["debug", "release"],
                "incremental" => [0, 1],
                "zipresults" => [0, 1],
                "abvsubset" => [0, 1],
                "scriptingBackend" => ["default"]
            },
            dependencies => ["WindowsStandaloneSupport"],
            platform => undef
        }
    );
}

sub BuildFacebookSupport
{
    my ($root, $targetPath, $options) = @_;
    my $config = lc($options->{codegen});

    JamRaw($root, "FacebookEditorExtensions", "-sCONFIG=$config");

    my $srcPath = "$targetPath/../WindowsStandaloneSupport";
    my @variations = (
        "Variations/win32_development_mono", "Variations/win32_nondevelopment_mono",
        "Variations/win64_development_mono", "Variations/win64_nondevelopment_mono",
        "Variations/win32_development_il2cpp", "Variations/win32_nondevelopment_il2cpp",
        "Variations/win64_development_il2cpp", "Variations/win64_nondevelopment_il2cpp"
    );

    foreach my $variation (@variations)
    {
        my $destDir = "$targetPath/$variation";

        # Copy standalone player for Facebook
        mkpath($destDir);
        copy("$srcPath/$variation/WindowsPlayer.exe", "$destDir/WindowsPlayer.exe");
        copy("$srcPath/$variation/UnityPlayer.dll", "$destDir/UnityPlayer.dll");

        # Sign the Facebook player
        SignFile("$destDir/WindowsPlayer.exe");
        SignFile("$destDir/UnityPlayer.dll");
    }

    PackageAndSendResultsToBuildServer($targetPath) if $options->{zipresults};
}

1;
