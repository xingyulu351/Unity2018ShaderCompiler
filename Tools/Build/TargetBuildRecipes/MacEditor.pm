package MacEditor;

use strict;
use warnings;
use File::Path qw(mkpath rmtree);
use File::Basename qw(dirname basename);
use File::Spec;
use File::Copy;
use lib File::Spec->rel2abs(dirname($0) . "/../../..") . '/External/Perl/lib';
use OTEE::BuildUtils qw(build_i386 svnGetRevision);
use lib File::Spec->rel2abs(dirname(__FILE__) . "/..");
use Carp qw (croak);
use MacTools qw (Cleanup LinkFileOrDirectory LinkFlattenedDirs);
use Tools qw (XBuild UCopy Jam AmRunningOnBuildServer ProgressMessage ReplaceText);
use Tools qw (PackageAndSendResultsToBuildServer BuildBundleFileList);
use Licenses qw(VerifyExternalLicenses BuildLicensesFile ExportExternalLicenses);

use Carp qw(croak carp);

# Choose which platform to use by default when building MacEditor.  Only
# used if no platform is specified.
my $MACEDITOR_DEFAULT_PLATFORM = "macosx64";

sub GetRegistrationData
{
    my ($buildFunction) = @_;

    return {
        macbuildfunction => $buildFunction,
        options => {
            "codegen" => ["debug", "release"],
            "incremental" => [0, 1],
            "lump" => [0, 1],
            "platform" => [],
            "jamArgs" => []
        },
        executable => "Unity.app",
    };
}

sub Register
{
    Main::RegisterTarget("MacEditor", GetRegistrationData(\&BuildMacEditor));
}

sub PrepareEditor
{
    my ($root, $targetPath, $options) = @_;

    my $app = "$targetPath/Unity.app";

    rmtree("$app/Contents/PkgInfo");
    mkpath("$app/Contents");
    system("echo \"APPLUNED\" > \"$app/Contents/PkgInfo\"") == 0 or croak "Failed creating pkginfo";
}

sub BuildMacEditor
{
    my ($root, $targetPath, $options) = @_;

    ProgressMessage("Verifying external licenses...");
    if (AmRunningOnBuildServer())
    {
        mkdir("$root/build/ReportedArtifacts");
        VerifyExternalLicenses(undef, "$root/build/ReportedArtifacts/license_report.csv");
    }
    else
    {
        VerifyExternalLicenses();
    }

    ProgressMessage("Exporting external licenses...");

    if (AmRunningOnBuildServer())
    {
        ExportExternalLicenses("$root/build/ReportedArtifacts/license_snapshot.zip");
    }

    PrepareEditor($root, $targetPath, $options);

    # If the user hasn't explicitly requested a specific platform,
    # build for default.
    my $platform = $options->{platform};
    if ($platform eq "")
    {
        $platform = $MACEDITOR_DEFAULT_PLATFORM;
    }

    # Build 32bit shader compiler and QuicktimeTools.
    ProgressMessage("Building tools");
    Jam($root, 'QTTools', $options->{codegen}, 'macosx32', $options->{incremental});

    # Build editor and tools (Binary2Text/WebExtract are not used by editor itself, and are not setup as dependency; but we need
    # them to be included in the editor distribution for end users).
    ProgressMessage("Building MacEditor");
    Jam(
        $root, 'MacEditor', $options->{codegen}, $platform,
        $options->{incremental}, "-sLUMP=$options->{lump}", 'Binary2Text', 'WebExtract',
        $options->{jamArgs}
    );

    ProgressMessage("Building legal.txt...");
    BuildLicensesFile("$root/build/MacEditor/Unity.app/Contents/Resources/legal.txt") and die("legal.txt build failed\n");

    my $unityVersion = $BuildConfig::unityVersion;
    my %svnInfo = svnGetRevision();
    my $svnRevision = "$svnInfo{Revision}";
    my $copyright = $BuildConfig::copyright;
    ReplaceText(
        "$root/build/MacEditor/Unity.app/Contents/Info.plist",
        "$root/build/MacEditor/Unity.app/Contents/Info.plist",
        (DEVELOPMENT_VERSION => $unityVersion, DEVELOPMENT_BUILD_NUMBER => $svnRevision, DEVELOPMENT_COPYRIGHT => $copyright)
    );

    system('chmod', '-R', 'o+rX,g+rX', "$root/build/MacEditor/Unity.app") and die("Failed to chmod Unity.app\n");

    rmtree("$targetPath/Unity.app/Content/MacOS/Temp");
    my $bundlePath = "$targetPath/Unity.app";
    print("Generating file list for $bundlePath\n");
    mkdir("$root/build/ReportedArtifacts");
    mkdir("$root/build/ReportedArtifacts/FileLists");
    BuildBundleFileList($bundlePath, "$root/build/ReportedArtifacts/FileLists/MacEditor.csv");

    PackageAndSendResultsToBuildServer($targetPath, "MacEditor", ".svn");
}

1;
