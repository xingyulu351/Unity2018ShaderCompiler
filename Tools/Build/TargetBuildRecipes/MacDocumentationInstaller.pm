package MacDocumentationInstaller;

use strict;
use warnings;
use File::Path qw(mkpath rmtree);
use File::Basename qw(dirname);
use File::Spec;
use File::Copy;
use Carp qw (croak);
use lib File::Spec->rel2abs(dirname($0) . "/..");
use MacTools qw (BuildPackage BuildMacProductInstaller SignPackage BuildAndPatchComponentPlist SetCustomFileIcon);
use Tools qw (SetupPlaybackEngines UCopy UMove AmRunningOnBuildServer ProgressMessage ReplaceText Jam);
use UserDocumentation;
use MacInstallerLicensePlugin qw(BuildMacInstallerLicensePlugin);

sub Register
{
    Main::RegisterTarget(
        "MacDocumentationInstaller",
        {
            macbuildfunction => \&BuildMacDocumentationInstaller,
            options => {
                "codegen" => ["debug", "release"],
                "incremental" => [0, 1],
                "platform" => []
            },
            dependencies => ["UserDocumentation",]
        }
    );
}

my $assemblyPath = "build/temp";
my $documentationAssemblyPath = "$assemblyPath/Files/DocumentationAssembly";

sub BuildDocs
{
    my ($root) = @_;

    ProgressMessage("Adding Documentation.html");
    mkdir($documentationAssemblyPath);
    UserDocumentation::WriteDocumentationHTML($documentationAssemblyPath);

    if (-e "$root/build/UserDocumentation")
    {
        ProgressMessage("Assembling UserDocumentation");

        # Copy manual and script reference into output, excluding info and XML
        UCopy("$root/build/UserDocumentation/Documentation/", "$documentationAssemblyPath/Documentation/");
        rmtree("$documentationAssemblyPath/Documentation/Documentation/info");
    }

}

sub BuildMacDocumentationInstaller
{
    my ($root, $targetPath, $options) = @_;

    my $assemblyPath = "$root/build/temp";

    if (AmRunningOnBuildServer())
    {
        rmtree("$assemblyPath");
    }

    my $pluginsPath = BuildMacInstallerLicensePlugin($root, $assemblyPath);

    mkpath($documentationAssemblyPath);
    BuildDocs($root);

    rmtree("$assemblyPath/InstallerSources");
    mkpath("$assemblyPath/InstallerSources");
    UCopy("$root/Tools/Installers/Documentation", "$assemblyPath/InstallerSources/Documentation");
    UCopy("$root/Tools/Licenses/License.rtf", "$assemblyPath/InstallerSources/Documentation/Resources/License.rtf");
    UCopy("$root/Tools/Installers/Unity/Resources/Welcome.rtf", "$assemblyPath/InstallerSources/Documentation/Resources/Welcome.rtf");
    UCopy("$root/Tools/Installers/InstallerAssets/background.tiff", "$assemblyPath/InstallerSources/Documentation/Resources/");

    ProgressMessage("Making DocCombiner");
    my $platform = ($options->{platform} or "macosx64");
    Jam($root, 'DocCombiner', $options->{codegen}, $platform, $options->{incremental});
    UCopy("$root/build/MacDocCombiner/DocCombiner", "$documentationAssemblyPath/Documentation/");
    system("chmod +x $documentationAssemblyPath/Documentation/DocCombiner");

    my $comp_plist = "$assemblyPath/InstallerSources/Documentation/component.plist";
    BuildAndPatchComponentPlist($documentationAssemblyPath, $comp_plist);

    BuildPackage(
        "$assemblyPath/Documentation.pkg.tmp",
        $documentationAssemblyPath, "/Applications/Unity", "$assemblyPath/InstallerSources/Documentation/Scripts",
        "com.unity3d.Documentation", $comp_plist
    );
    my @packages = ("$assemblyPath/Documentation.pkg.tmp");
    BuildMacProductInstaller(
        targetPath => "$targetPath/Documentation.pkg",
        sourceFiles => "$assemblyPath/InstallerSources/Documentation",
        packageList => \@packages,
        pluginsPath => $pluginsPath
    );
    SetCustomFileIcon("$targetPath/Documentation.pkg", "$root/Tools/Installers/InstallerAssets/installer.icns");
}

1;
