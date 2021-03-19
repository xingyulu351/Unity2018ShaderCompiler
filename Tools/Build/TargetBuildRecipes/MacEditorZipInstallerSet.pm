package MacEditorZipInstallerSet;

use strict;
use warnings;
use File::Path qw(mkpath rmtree);
use File::Basename qw(dirname basename);
use File::Spec;
use File::Copy;
use lib File::Spec->rel2abs(dirname($0) . "/../../..") . '/External/Perl/lib';
use OTEE::BuildUtils qw(build_i386 svnGetRevision);
use lib File::Spec->rel2abs(dirname(__FILE__) . "/..");
use File::chdir;
use Carp qw (croak);
use MacTools qw (Cleanup LinkFileOrDirectory LinkFlattenedDirs);
use Tools qw (XBuild UCopy Jam AmRunningOnBuildServer ProgressMessage ReplaceText);
use Tools qw (PackageAndSendResultsToBuildServer BuildBundleFileList);
use Licenses qw(VerifyExternalLicenses BuildLicensesFile ExportExternalLicenses);
use Carp qw(croak carp);

my $MACEDITOR_DEFAULT_PLATFORM = "macosx64";

sub Register
{
    Main::RegisterTarget(
        "MacEditorZipInstallerSet",
        {
            macbuildfunction => \&BuildMacEditorZipInstallerSet,
            dependencies => [],
        }
    );
}

sub BuildMacEditorZip
{
    my ($root, $zipApp, $targetPath, $editorName) = @_;

    my $zipName = "$editorName.zip";
    my $editorFiles = "$root/build/MacEditor/Unity.app";
    my $bugReporterFiles = "$root/build/MacEditor/Unity.app/Contents/BugReporter/Unity Bug Reporter.app";
    my $defaultPlaybackEngineFiles = "$root/build/MacStandaloneSupport/";
    ProgressMessage("Zipping up Mac Editor Zip Installer");

    my @list = (
        "$zipApp", "a", "-r", "-xr!.git",
        "-xr!.hg", "-xr!.hglf", "-xr!*.testlog", "-xr!*.ilk",
        "-xr!*.pdb", "-x!Temp", "-x!etc", "-xr0!*node_modules/*/history",
        "-xr0!*node_modules/*/changelog", "-xr0!*node_modules/*/*.md", "-xr0!*node_modules/*/*.markdown", "-xr0!*node_modules/*/readme.*",
        "-xr0!*node_modules/*/LICENSE.*", "-xr0!*node_modules/*/LICENSE", "-xr0!*node_modules/*/tests", "-xr0!*node_modules/*/*test*.js",
        "-xr0!*node_modules/*/test", "-xr0!*node_modules/*/.npmignore", "-xr0!*node_modules/*/appveyor.yml", "-xr0!*node_modules/*/.travis.yml",
        "$targetPath/$zipName", "$editorFiles", "$bugReporterFiles", "$defaultPlaybackEngineFiles"
    );

    #create a zip archive
    system(@list) and croak("Failed to zip up mac editor");

    # move the mac standalonesupport to its rightful place
    system("$zipApp", "rn", "$targetPath/$zipName", "MacStandaloneSupport/", "Unity.app/Contents/PlaybackEngines/MacStandaloneSupport/")
        and croak("Failed to move Mac Standalone Support");

    my @etcs = ("Contents/Mono/etc", "Contents/MonoBleedingEdge/etc");
    foreach (@etcs)
    {
        if (-e "$editorFiles/$_")
        {
            system("$zipApp", "a", "$targetPath/$zipName", "$editorFiles/$_") and croak("Failed to zip up $editorFiles/$_");
            system("$zipApp", "rn", "$targetPath/$zipName", "etc", "Unity.app/$_")
                and croak("Failed to rename etc to $_");
        }
    }

    my @playeretcs = (
        "Variations/macosx64_development_mono/UnityPlayer.app/Contents/Mono/etc",
        "Variations/macosx64_development_mono/UnityPlayer.app/Contents/MonoBleedingEdge/etc",
        "Variations/macosx64_nondevelopment_mono/UnityPlayer.app/Contents/Mono/etc",
        "Variations/macosx64_nondevelopment_mono/UnityPlayer.app/Contents/MonoBleedingEdge/etc"
    );
    foreach (@playeretcs)
    {
        if (-e "$defaultPlaybackEngineFiles/$_")
        {
            system("$zipApp", "a", "$targetPath/$zipName", "$defaultPlaybackEngineFiles/$_") and croak("Failed to zip up $defaultPlaybackEngineFiles/$_");
            system("$zipApp", "rn", "$targetPath/$zipName", "etc", "Unity.app/Contents/PlaybackEngines/MacStandaloneSupport/$_")
                and croak("Failed to rename etc to $_");
        }
    }
}

sub BuildMacComponentZip
{
    my ($zipApp, $targetPath, $componentName, $componentFolder) = @_;
    my $zipName = "$componentName.zip";
    ProgressMessage("Zipping up $componentName");

    if (-d $componentFolder)
    {

        # verify if there is a ZippedForTeamCity.zip file already
        if (-e "$componentFolder/ZippedForTeamCity.zip")
        {
            copy("$componentFolder/ZippedForTeamCity.zip", "$targetPath/$zipName") or die "Copy of the zip failed: $!";
        }
        else
        {
            system("$zipApp", "a", "-r", "-xr!.git", "-xr!.hg", "-xr!.hglf", "$targetPath/$zipName", "$componentFolder/*")
                and croak("Failed to zip up component from $componentFolder");
        }
    }
}

sub BuildMacEditorZipInstallerSet
{
    my ($root, $targetPath, $options) = @_;
    ProgressMessage("Building Mac Editor Zip Installer Set");

    # At this point, this script expects to have all dependencies,
    # at the following locations:
    #
    # Unity Editor: build/MacEditor/
    # User Documentation: build/UserDocumentation
    # Standard Assets: build/StandardAssets
    # Example Project: build/ExampleProject
    # Mono Develop: $targetPath/MonoDevelop.zip !!
    # Android Build Support: build/AndroidPlayer
    # iOS Build Support: build/iOSSupport
    # TvOS Build Support: build/AppleTVSupport
    # Winbdows Build Support: build/windowsstandalonesupport
    # Linux Build Support: build/LinuxStandaloneSupport
    # Mac Build Support: build/MacStandaloneSupport
    # Tizen Build Support: build/TizenPlayer
    # Vuforia Build Support: build/Vuforia
    # WebGL Build Support: build/WebGLSupport
    # Facebook Build Support: build/Facebook

    my $zipApp = "$root/External/7z/osx/7za";
    BuildMacEditorZip($root, $zipApp, $targetPath, "MacEditorZipInstaller");

    # create a zip for each component
    my %components = (
        UserDocumentation => "$root/build/UserDocumentation",
        StandardAssets => "$root/build/StandardAssets",
        ExampleProject => "$root/build/ExampleProject",
        AndroidBuildSupport => "$root/build/AndroidPlayer",
        iOSBuildSupport => "$root/build/iOSSupport",
        TvOSBuildSupport => "$root/build/AppleTVSupport",
        WindowsStandaloneBuildSupport => "$root/build/windowsstandalonesupport",
        LinuxStandaloneBuildSupport => "$root/build/LinuxStandaloneSupport",
        MacStandaloneBuildSupport => "$root/build/MacStandaloneSupport",
        TizenBuildSupport => "$root/build/TizenPlayer",
        VuforiaBuildSupport => "$root/build/VuforiaSupport",
        WebGLBuildSupport => "$root/build/WebGLSupport",
        FacebookBuildSupport => "$root/build/Facebook",
    );

    for my $component (keys %components)
    {
        BuildMacComponentZip($zipApp, $targetPath, $component, $components{$component});
    }
}
1;
