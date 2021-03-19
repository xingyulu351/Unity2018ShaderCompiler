package Il2CppUpdatePackage;

use strict;
use warnings;
use File::Path qw(mkpath rmtree);
use File::Basename qw(dirname basename);
use File::Spec;
use File::Copy;
use File::chdir;
use Carp qw (croak);
use lib File::Spec->rel2abs(dirname($0) . "/..");
use WinTools qw (SignFile);
use MacTools qw (BuildPackage BuildInstallerFromPackages SignPackage);
use Tools qw (UCopy UMove AmRunningOnBuildServer ReplaceText Jam);
use BuildConfig;
our $unityVersion = $BuildConfig::unityVersion;

sub Register
{
    Main::RegisterTarget(
        "Il2CppUpdatePackage",
        {
            windowsbuildfunction => \&BuildWindowsIl2CppUpdatePackage,
            macbuildfunction => \&BuildMacIl2CppUpdatePackage,
            dependencies => []
        }
    );
}

sub GetIl2cppPackageVersion
{
    my ($root) = @_;
    my $il2cppJamFile = "$root/External/il2cpp/IL2CPP.jam";

    my $packageVersion = undef;

    open(FH, $il2cppJamFile) || die "Couldn't open $il2cppJamFile\n";

    while (<FH>)
    {
        if ($_ =~ /^IL2CPP_PACKAGE_VERSION\s*\?=\s*(\d+)\.(\d+)\.(\d+)\s*;/igs)
        {
            $packageVersion = "$1.$2.$3";
        }
    }

    close(FH);

    die "Couldn't extract version number from $il2cppJamFile" unless defined $packageVersion;

    return $packageVersion;
}

sub BuildMacIl2CppUpdatePackage
{
    my ($root, $targetPath, $options) = @_;
    my $destPath = "$root/build/Il2CppUpdatePackage";
    my $tempPath = "$root/build/temp/il2cpp-update-package";
    my ($major, $minor) = $unityVersion =~ /^(\d+)\.(\d+)\./igs;
    my $packageVersion = GetIl2cppPackageVersion($root);

    rmtree("$destPath");
    mkpath("$destPath");
    mkpath("$tempPath");
    mkpath("$tempPath/Scripts");

    Jam($root, 'IL2CPP', "debug", 'macosx64', 0, "-sIL2CPP_PACKAGE_VERSION=$packageVersion");

    UCopy("build/MacEditor/Unity.app/Contents/Frameworks/il2cpp", "$tempPath");

    ReplaceText(
        "$root/External/il2cpp/osx-installer/distribution-tpl.xml", "$tempPath/distribution.xml",
        UNITY_VERSION => "$major.$minor",
        IL2CPP_VERSION => $packageVersion
    );
    ReplaceText(
        "$root/External/il2cpp/osx-installer/postinstall-tpl", "$tempPath/Scripts/postinstall",
        UNITY_VERSION => "$major.$minor",
        IL2CPP_VERSION => $packageVersion
    );

    system("chmod +x '$tempPath/Scripts/postinstall'");

    system("pkgbuild --root '$tempPath/il2cpp' --scripts '$tempPath/Scripts' --identifier com.unity3d.il2cpp '$tempPath/il2cpp.pkg.tmp'");
    system("productbuild --distribution '$tempPath/distribution.xml' --package-path '$tempPath' '$destPath/il2cpp-$packageVersion.pkg'");
}

sub BuildWindowsIl2CppUpdatePackage
{
    my ($root, $targetPath, $options) = @_;
    my $destPath = "$root/build/Il2CppUpdatePackage";
    my $tempPath = "$root/build/temp/il2cpp-update-package";
    my ($major, $minor) = $unityVersion =~ /^(\d+)\.(\d+)\./igs;
    my $packageVersion = GetIl2cppPackageVersion($root);

    my $finalInstallerFile = "$tempPath/il2cpp-$packageVersion.exe";

    rmtree("$destPath");
    mkpath("$destPath");
    mkpath("$tempPath");

    Jam($root, 'IL2CPP', "debug", 'win32', 0, "-sIL2CPP_PACKAGE_VERSION=$packageVersion");

    UCopy("build/WindowsEditor/Data/il2cpp", "$tempPath/files");

    ReplaceText(
        "$root/External/il2cpp/win-installer/setup-il2cpp-tpl.nsi", "$tempPath/setup-il2cpp.nsi",
        RPL_UNITY_VERSION => "$major.$minor",
        RPL_COPYRIGHT => $BuildConfig::copyright,
        RPL_IL2CPP_VERSION => $packageVersion,
        RPL_BACKGROUND_IMG => "$root\\Tools\\Installers\\WindowsEditor\\Background.bmp",
        RPL_LICENSE_FILE => "$root\\Tools\\Licenses\\License.txt"
    );

    # local $CWD = "$root/Tools/Installers/TargetSupport";

    system("$root/External/Installers/NSIS/builds/makensis.exe", "/V2", "$tempPath/setup-il2cpp.nsi") eq 0 or croak("Failing creating final installer");
    if (not -e $finalInstallerFile) { croak("Creating installer returned succesfully, however the produced installer is not found at $finalInstallerFile\n"); }

    SignFile($finalInstallerFile);

    UCopy($finalInstallerFile, "$destPath/il2cpp-$packageVersion.exe");

}

1;
