package MacInstallerLicensePlugin;
use strict;
use warnings;
use File::Path;
use Tools qw(UCopy);
use OTEE::BuildUtils qw(build_x86_64);

require Exporter;
our @ISA = qw(Exporter);
our @EXPORT_OK = qw(BuildMacInstallerLicensePlugin);

sub BuildMacInstallerLicensePlugin
{
    my $root = shift;
    my $assemblyPath = shift;
    my $license_url = (shift or "");

    #build_x86_64 ("$root/Tools/Installers/MacInstallerLicensePlugin/InstallerLicensePage.xcodeproj", 'InstallerLicensePage');
    system(
        'xcodebuild',
        -project => "$root/Tools/Installers/MacInstallerLicensePlugin/InstallerLicensePage.xcodeproj",
        -target => 'InstallerLicensePage',
        -configuration => 'Release',
        "INSTALLER_LICENSE_URL='$license_url'", 'ARCHS=x86_64'
    ) and die("Xcode could not build the MacInstallerLicensePlugin project.\n");

    rmtree("$assemblyPath/MacInstallerPlugins");
    mkpath("$assemblyPath/MacInstallerPlugins");
    UCopy("$root/Tools/Installers/MacInstallerLicensePlugin/build/Release/InstallerLicensePage.bundle", "$assemblyPath/MacInstallerPlugins/");
    UCopy("$root/Tools/Installers/MacInstallerLicensePlugin/InstallerLicensePage/InstallerSections.plist", "$assemblyPath/MacInstallerPlugins/");
    return "$assemblyPath/MacInstallerPlugins";
}
