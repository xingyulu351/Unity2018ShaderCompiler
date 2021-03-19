package MacMonoDevelopInstaller;

use strict;
use warnings;
use File::Path qw(mkpath rmtree);
use File::Basename qw(dirname basename);
use File::Spec;
use File::Copy;
use Carp qw (croak);
use lib File::Spec->rel2abs(dirname($0) . "/..");
use MacTools qw (BuildPackage BuildMacProductInstaller SignPackage BuildAndPatchComponentPlist SetCustomFileIcon);
use Tools qw (UCopy UMove AmRunningOnBuildServer ReplaceText ProgressMessage);
use MacInstallerLicensePlugin qw(BuildMacInstallerLicensePlugin);

sub Register
{
    Main::RegisterTarget(
        "MacMonoDevelopInstaller",
        {
            macbuildfunction => \&BuildMacMonoDevelopInstaller,
            dependencies => []
        }
    );
}

sub BuildMacMonoDevelopInstaller
{
    my ($root, $targetPath, $options) = @_;

    my $assemblyPath = "$root/build/temp";

    if (AmRunningOnBuildServer())
    {
        rmtree("$assemblyPath");
    }

    my $pluginsPath = BuildMacInstallerLicensePlugin($root, $assemblyPath);

    rmtree("$assemblyPath/InstallerSources");
    mkpath("$assemblyPath/InstallerSources");
    UCopy("$root/Tools/Installers/MacMonoDevelop", "$assemblyPath/InstallerSources/MacMonoDevelop");

    if (!-e "$root/build/MacMonoDevelop/MonoDevelop.app" && -e "$root/MonoDevelopBinaries/MonoDevelop.app.tar.gz")
    {
        ProgressMessage("Unpacking $root/MonoDevelopBinaries/MonoDevelop.app.tar.gz");
        system("mkdir -p $root/build/MacMonoDevelop && cd $root/build/MacMonoDevelop && tar zxf $root/MonoDevelopBinaries/MonoDevelop.app.tar.gz");
        $? == 0 or die("Unpacking $root/MonoDevelopBinaries/MonoDevelop.app.tar.gz failed\n");
    }

    my $monodevpath = "$root/build/MacMonoDevelop/MonoDevelop.app";
    if (-e $monodevpath)
    {
        rmtree("$assemblyPath/MonoDevelop");
        mkdir("$assemblyPath/MonoDevelop");

        # Don't follow symlinks or else the mono frameworks gets multiplied 5x times.
        system("cp", "-Rfp", "$monodevpath", "$assemblyPath/MonoDevelop/MonoDevelop.app");
    }
    else
    {
        die("MonoDevelop not found at $monodevpath\n");
    }

    UCopy("$root/Tools/Installers/Unity/Resources/Welcome.rtf", "$assemblyPath/InstallerSources/MacMonoDevelop/Resources/Welcome.rtf");
    UCopy("$root/Tools/Licenses/License.rtf", "$assemblyPath/InstallerSources/MacMonoDevelop/Resources/License.rtf");
    UCopy("$root/Tools/Installers/InstallerAssets/background.tiff", "$assemblyPath/InstallerSources/MacMonoDevelop/Resources/");

    my $comp_plist = "$assemblyPath/InstallerSources/MacMonoDevelop/component.plist";
    BuildAndPatchComponentPlist("$assemblyPath/MonoDevelop", $comp_plist);

    BuildPackage(
        "$assemblyPath/UnityMonoDevelop.pkg.tmp",
        "$assemblyPath/MonoDevelop", "/Applications/Unity", "$assemblyPath/InstallerSources/MacMonoDevelop/Scripts",
        "com.unity3d.MonoDevelop", $comp_plist
    );
    my @packages = ("$assemblyPath/UnityMonoDevelop.pkg.tmp");
    BuildMacProductInstaller(
        targetPath => "$targetPath/UnityMonoDevelop.pkg",
        sourceFiles => "$assemblyPath/InstallerSources/MacMonoDevelop",
        packageList => \@packages,
        pluginsPath => $pluginsPath
    );
    SetCustomFileIcon("$targetPath/UnityMonoDevelop.pkg", "$root/Tools/Installers/InstallerAssets/installer.icns");
}

1;
