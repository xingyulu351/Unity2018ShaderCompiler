package WindowsEditor;

use strict;
use warnings;

use File::Basename qw(dirname basename fileparse);
use File::Copy;
use File::Spec;
use File::Path;
use File::Glob;
use File::Find;
use Carp qw(croak carp);
use File::chdir;
use Tools qw (XBuild UCopy UMove Jam AmRunningOnBuildServer PackageAndSendResultsToBuildServer ProgressMessage BuildBundleFileList);
use OTEE::BuildUtils qw(svnGetRevision);
use WinTools qw (AddSourceServerToPDB SignFile);
use Licenses qw(VerifyExternalLicenses BuildLicensesFile);

# Choose which platform to use by default when building WindowsEditor.  Only
# used if no platform is specified.
my $WINDOWSEDITOR_DEFAULT_PLATFORM = "win64";

sub GetRegistrationData
{
    my ($buildFunction) = @_;

    return {
        windowsbuildfunction => $buildFunction,
        options => {
            "codegen" => ["debug", "release"],
            "developmentPlayer" => [0, 1],
            "incremental" => [0, 1],
            "lump" => [0, 1],
            "platform" => [],
            "jamArgs" => []
        },
        executable => "Unity.exe",
        requiresStdoutRedirect => 1,
    };
}

sub Register
{
    Main::RegisterTarget("WindowsEditor", GetRegistrationData(\&BuildWindowsEditor));
}

sub PrepareWindowsEditor
{
    my ($root, $targetPath, $options) = @_;
}

sub BuildWindowsEditor
{
    my ($root, $targetPath, $options) = @_;

    my $jamArgs = $options->{jamArgs};

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

    my $platform = $options->{platform};
    if ($platform eq "")
    {
        $platform = $WINDOWSEDITOR_DEFAULT_PLATFORM;
    }

    PrepareWindowsEditor($root, $targetPath, $options);

    # Build tools we don't support in 64bit in 32bit.
    if ($platform eq 'win64')
    {
        ProgressMessage("Building Tools");
        Jam($root, 'QTTools', $options->{codegen}, 'win32', $options->{incremental});
    }

    # Build editor and tools (Binary2Text/WebExtract are not used by editor itself, and are not setup as dependency; but we need
    # them to be included in the editor distribution for end users).
    ProgressMessage("Building WindowsEditor");
    Jam($root, 'Editor', $options->{codegen}, $platform, $options->{incremental}, "-sLUMP=$options->{lump}", $jamArgs, 'Binary2Text', 'WebExtract');

    my @unwanted = glob("$targetPath/*.lib");
    push @unwanted, glob("$targetPath/*.ilk");
    push @unwanted, glob("$targetPath/*.exp");
    push @unwanted, glob("$targetPath/Data/Tools/*.ilk");
    for my $file (@unwanted)
    {
        unlink($file);
    }

    my @pdbs = ();

    # filter out pdbs from lib/mono since the Mono installs contain portable PDB files and the tool errors on these currently
    find(
        {
            wanted => sub
            {
                if ($File::Find::dir =~ /\lib\/mono/) { return }
                /\.pdb$/ && push @pdbs, $File::Find::name;
            },
            no_chdir => 1
        },
        $targetPath
    );
    AddSourceServerToPDB($root, \@pdbs);

    ProgressMessage("Building legal.txt...");
    BuildLicensesFile("$root/build/WindowsEditor/Data/Resources/legal.txt") and die "legal.txt build failed\n";

    ProgressMessage("Signing Unity.exe");
    SignFile("$targetPath/Unity.exe");

    ProgressMessage("Signing various tools");

    SignFile("$targetPath/Data/Tools/UnityYAMLMerge.exe");
    SignFile("$targetPath/Data/Tools/UnityShaderCompiler.exe");
    SignFile("$targetPath/Data/Tools/JobProcess.exe");
    SignFile("$targetPath/Data/Tools/UnwrapCL.exe");
    SignFile("$targetPath/Data/Tools/UnityCrashHandler64.exe");
    SignFile("$targetPath/Data/Resources/PackageManager/Server/UnityPackageManager.exe");

    # For submission to winquals, but must remove from installer!
    UCopy("$root/External/Microsoft/winqual/winqual.exe", "$targetPath/Temp/");
    SignFile("$targetPath/Temp/winqual.exe");

    my $bundlePath = "$targetPath";
    print("Generating file list for $bundlePath\n");
    mkdir("$root/build/ReportedArtifacts");
    mkdir("$root/build/ReportedArtifacts/FileLists");
    BuildBundleFileList($bundlePath, "$root/build/ReportedArtifacts/FileLists/WindowsEditor.csv");

    PackageAndSendResultsToBuildServer($targetPath, "WindowsEditor");
}

1;
