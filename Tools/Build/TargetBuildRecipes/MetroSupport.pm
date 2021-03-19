package MetroSupport;

use File::Basename qw(dirname basename fileparse);
use File::Copy;
use File::Spec;
use File::Path;
use File::Glob;
use File::Find;
use Carp qw(croak carp);
use File::chdir;
use Tools qw (Jam JamRaw UCopy ProgressMessage AmRunningOnBuildServer IsWindows8OrNewer);
use Tools qw (PackageAndSendResultsToBuildServer);
use Tools qw (DeleteRecursive);
use OTEE::BuildUtils qw(svnGetRevision);
use WinTools qw (AddSourceServerToPDB SignFile);

use strict;
use warnings;

sub Register()
{
    Main::RegisterTarget(
        "MetroSupport",
        {
            windowsbuildfunction => \&BuildMetroSupport,
            options => {
                "codegen" => ["debug", "release", "allPlayersUAP", "devPlayersUAP"],
                "developmentPlayer" => [0, 1],
                "incremental" => [0, 1],
                "jamArgs" => []
            }
        }
    );
}

sub BuildMetroEditorExtensions
{
    my ($root, $jamArgs) = @_;
    ProgressMessage("Building Metro editor extensions.");
    Jam($root, "MetroEditorExtensions", "", 'win64', 1, $jamArgs);
}

sub PrepareMetroSupport
{
    if (IsWindows8OrNewer() == 0)
    {
        ProgressMessage("Skipping BuildMetroSupport. Windows 8+ is required.");
        return;
    }

    my ($root, $targetPath, $options) = @_;
    mkdir("$targetPath");

    print("Erasing $root/Runtime/ExportGenerated/MetroSupport directory\n");
    rmtree("$root/Runtime/ExportGenerated/MetroSupport");
    print("Erasing $root/temp/MetroSupport\n");
    rmtree("$root/temp/MetroSupport");

    # Create directory structure before jam does it, because sometimes jam will fail to create these, I don't know what are the exact repro steps
    mkdir("$targetPath/Players");

    # Setup build version
    my %svnInfo = svnGetRevision();
    my $svnRevision = $svnInfo{Revision};
    my $svnNumericRevision = $svnInfo{NumericRevision};
    my $finalVersion = "$svnNumericRevision-$svnRevision";

    my $filePath = "$targetPath/version.txt";
    mkpath(dirname($filePath));
    open(VERSION_FILE, ">$filePath") or croak("Unable to write $filePath");
    print(VERSION_FILE $finalVersion);
    close(VERSION_FILE);
}

sub BuildUAPSupportConfig
{
    my ($root, $params) = @_;

    ProgressMessage("Building UAP player.");
    print "Build params: " . $params . "\n";
    system("CALL \"$root/jam.bat\" \"-sNDA_PLATFORM_ROOT=$root/Projects/UAP\" " . $params) == 0 or croak("Unable to build UAP player.");
}

sub ClearUnusedFiles
{
    my ($targetPath) = @_;

    DeleteRecursive($targetPath, "*.ilk", 1);
    DeleteRecursive($targetPath, "*.exp", 1);
    DeleteRecursive($targetPath, "*.pri", 1);

    # .lib can either be import libraries or static libraries
    # We don't ship any static libraries today, and we don't
    # export any symbols from our native DLLs to be used for import libraries
    DeleteRecursive($targetPath, "*.lib", 1);

    # Incremental LTCG temporary files
    DeleteRecursive($targetPath, "*.ipdb", 1);
    DeleteRecursive($targetPath, "*.iobj", 1);

    # Visual Studio copies this one when building WinRTBridge. We don't need it here.
    DeleteRecursive("$targetPath/Players", "*UnityEngine.*", 1);
}

sub SignBinariesRecursive
{
    my ($dir, $pattern, $filenamesToSkip) = @_;

    local *DIR;
    opendir(DIR, $dir) or croak("Could not open directory $dir: $!");

    my @patterns = split(/\s/, $pattern);

    foreach my $file (File::Glob::glob("$dir/$pattern"))
    {
        my $exists = -e $file;
        my $isDir = -d $file;
        if ($exists && !$isDir)
        {
            SignFile($file);
        }
    }

    while (defined(my $file = readdir(DIR)))
    {
        # Only recurse on directories, which do not start with '.', and skip symbolic links
        if (-d "$dir/$file" && !(-l "$dir/$file") && ($file !~ /^\.{1,2}$/))
        {
            SignBinariesRecursive("$dir/$file", $pattern);
        }
    }

    closedir(DIR);
}

sub SignBinaries
{
    print "Signing binaries\n";
    my ($root) = @_;

    # Editor extensions
    SignBinariesRecursive("$root", "UnityEditor.WSA.Extensions*.dll");

    # Managed assemblies
    SignBinariesRecursive("$root/Managed", "*.dll");

    # Players
    SignBinariesRecursive("$root/Players", "*.dll");
    SignBinariesRecursive("$root/Players", "*.winmd");

    # Tools
    SignBinariesRecursive("$root/Tools", "*.dll");
    SignBinariesRecursive("$root/Tools", "AssemblyConverter.exe");
    SignBinariesRecursive("$root/Tools", "MetroPlayerRunner.exe");
    SignBinariesRecursive("$root/Tools", "rrw.exe");
    SignBinariesRecursive("$root/Tools", "SerializationWeaver.exe");
    SignBinariesRecursive("$root/Tools", "WindowsPhonePlayerRunner.exe");

    print "Signing binaries done\n";
}

sub BuildMetroSupport
{
    if (IsWindows8OrNewer() == 0)
    {
        ProgressMessage("Skipping BuildMetroSupport. Windows 8+ is required.");
        return;
    }

    my ($root, $targetPath, $options) = @_;
    my $developmentplayer = $options->{developmentPlayer};
    my $incremental = $options->{incremental};
    my $jamArgs = $options->{jamArgs};
    PrepareMetroSupport($root, $targetPath, $options);

    BuildMetroEditorExtensions($root, $jamArgs);
    if ($options->{codegen} eq 'allPlayersUAP')
    {
        BuildUAPSupportConfig($root, "-sPLATFORM=uap_arm -sCONFIG=debug -sSCRIPTING_BACKEND=dotnet $jamArgs MetroSupport");
        BuildUAPSupportConfig($root, "-sPLATFORM=uap_arm -sCONFIG=release -sSCRIPTING_BACKEND=dotnet $jamArgs MetroSupport");
        BuildUAPSupportConfig($root, "-sPLATFORM=uap_arm -sCONFIG=master -sSCRIPTING_BACKEND=dotnet $jamArgs MetroSupport");

        BuildUAPSupportConfig($root, "-sPLATFORM=uap_x64 -sCONFIG=debug -sSCRIPTING_BACKEND=dotnet $jamArgs MetroSupport");
        BuildUAPSupportConfig($root, "-sPLATFORM=uap_x64 -sCONFIG=release -sSCRIPTING_BACKEND=dotnet $jamArgs MetroSupport");
        BuildUAPSupportConfig($root, "-sPLATFORM=uap_x64 -sCONFIG=master -sSCRIPTING_BACKEND=dotnet $jamArgs MetroSupport");

        BuildUAPSupportConfig($root, "-sPLATFORM=uap_x86 -sCONFIG=debug -sSCRIPTING_BACKEND=dotnet $jamArgs MetroSupport");
        BuildUAPSupportConfig($root, "-sPLATFORM=uap_x86 -sCONFIG=release -sSCRIPTING_BACKEND=dotnet $jamArgs MetroSupport");
        BuildUAPSupportConfig($root, "-sPLATFORM=uap_x86 -sCONFIG=master -sSCRIPTING_BACKEND=dotnet $jamArgs MetroSupport");
    }
    elsif ($options->{codegen} eq 'allPlayersUAPIL2CPP')
    {
        BuildUAPSupportConfig($root, "-sPLATFORM=uap_arm -sCONFIG=debug -sSCRIPTING_BACKEND=il2cpp $jamArgs MetroSupport");
        BuildUAPSupportConfig($root, "-sPLATFORM=uap_arm -sCONFIG=release -sSCRIPTING_BACKEND=il2cpp $jamArgs MetroSupport");
        BuildUAPSupportConfig($root, "-sPLATFORM=uap_arm -sCONFIG=master -sSCRIPTING_BACKEND=il2cpp $jamArgs MetroSupport");

        BuildUAPSupportConfig($root, "-sPLATFORM=uap_arm64 -sCONFIG=debug -sSCRIPTING_BACKEND=il2cpp $jamArgs MetroSupport");
        BuildUAPSupportConfig($root, "-sPLATFORM=uap_arm64 -sCONFIG=release -sSCRIPTING_BACKEND=il2cpp $jamArgs MetroSupport");
        BuildUAPSupportConfig($root, "-sPLATFORM=uap_arm64 -sCONFIG=master -sSCRIPTING_BACKEND=il2cpp $jamArgs MetroSupport");

        BuildUAPSupportConfig($root, "-sPLATFORM=uap_x64 -sCONFIG=debug -sSCRIPTING_BACKEND=il2cpp $jamArgs MetroSupport");
        BuildUAPSupportConfig($root, "-sPLATFORM=uap_x64 -sCONFIG=release -sSCRIPTING_BACKEND=il2cpp $jamArgs MetroSupport");
        BuildUAPSupportConfig($root, "-sPLATFORM=uap_x64 -sCONFIG=master -sSCRIPTING_BACKEND=il2cpp $jamArgs MetroSupport");

        BuildUAPSupportConfig($root, "-sPLATFORM=uap_x86 -sCONFIG=debug -sSCRIPTING_BACKEND=il2cpp $jamArgs MetroSupport");
        BuildUAPSupportConfig($root, "-sPLATFORM=uap_x86 -sCONFIG=release -sSCRIPTING_BACKEND=il2cpp $jamArgs MetroSupport");
        BuildUAPSupportConfig($root, "-sPLATFORM=uap_x86 -sCONFIG=master -sSCRIPTING_BACKEND=il2cpp $jamArgs MetroSupport");
    }
    elsif ($options->{codegen} eq 'allPlayersUAPDotNetX86')
    {
        BuildUAPSupportConfig($root, "-sPLATFORM=uap_x86 -sCONFIG=debug -sSCRIPTING_BACKEND=dotnet $jamArgs MetroSupport");
        BuildUAPSupportConfig($root, "-sPLATFORM=uap_x86 -sCONFIG=release -sSCRIPTING_BACKEND=dotnet $jamArgs MetroSupport");
        BuildUAPSupportConfig($root, "-sPLATFORM=uap_x86 -sCONFIG=master -sSCRIPTING_BACKEND=dotnet $jamArgs MetroSupport");
    }
    elsif ($options->{codegen} eq 'allPlayersUAPDotNetX64')
    {
        BuildUAPSupportConfig($root, "-sPLATFORM=uap_x64 -sCONFIG=debug -sSCRIPTING_BACKEND=dotnet $jamArgs MetroSupport");
        BuildUAPSupportConfig($root, "-sPLATFORM=uap_x64 -sCONFIG=release -sSCRIPTING_BACKEND=dotnet $jamArgs MetroSupport");
        BuildUAPSupportConfig($root, "-sPLATFORM=uap_x64 -sCONFIG=master -sSCRIPTING_BACKEND=dotnet $jamArgs MetroSupport");
    }
    elsif ($options->{codegen} eq 'allPlayersUAPDotNetARM')
    {
        BuildUAPSupportConfig($root, "-sPLATFORM=uap_arm -sCONFIG=debug -sSCRIPTING_BACKEND=dotnet $jamArgs MetroSupport");
        BuildUAPSupportConfig($root, "-sPLATFORM=uap_arm -sCONFIG=release -sSCRIPTING_BACKEND=dotnet $jamArgs MetroSupport");
        BuildUAPSupportConfig($root, "-sPLATFORM=uap_arm -sCONFIG=master -sSCRIPTING_BACKEND=dotnet $jamArgs MetroSupport");
    }
    elsif ($options->{codegen} eq 'allPlayersUAPIL2CPPX86')
    {
        BuildUAPSupportConfig($root, "-sPLATFORM=uap_x86 -sCONFIG=debug -sSCRIPTING_BACKEND=il2cpp $jamArgs MetroSupport");
        BuildUAPSupportConfig($root, "-sPLATFORM=uap_x86 -sCONFIG=release -sSCRIPTING_BACKEND=il2cpp $jamArgs MetroSupport");
        BuildUAPSupportConfig($root, "-sPLATFORM=uap_x86 -sCONFIG=master -sSCRIPTING_BACKEND=il2cpp $jamArgs MetroSupport");
    }
    elsif ($options->{codegen} eq 'allPlayersUAPIL2CPPX64')
    {
        BuildUAPSupportConfig($root, "-sPLATFORM=uap_x64 -sCONFIG=debug -sSCRIPTING_BACKEND=il2cpp $jamArgs MetroSupport");
        BuildUAPSupportConfig($root, "-sPLATFORM=uap_x64 -sCONFIG=release -sSCRIPTING_BACKEND=il2cpp $jamArgs MetroSupport");
        BuildUAPSupportConfig($root, "-sPLATFORM=uap_x64 -sCONFIG=master -sSCRIPTING_BACKEND=il2cpp $jamArgs MetroSupport");
    }
    elsif ($options->{codegen} eq 'allPlayersUAPIL2CPPARM')
    {
        BuildUAPSupportConfig($root, "-sPLATFORM=uap_arm -sCONFIG=debug -sSCRIPTING_BACKEND=il2cpp $jamArgs MetroSupport");
        BuildUAPSupportConfig($root, "-sPLATFORM=uap_arm -sCONFIG=release -sSCRIPTING_BACKEND=il2cpp $jamArgs MetroSupport");
        BuildUAPSupportConfig($root, "-sPLATFORM=uap_arm -sCONFIG=master -sSCRIPTING_BACKEND=il2cpp $jamArgs MetroSupport");
    }
    elsif ($options->{codegen} eq 'allPlayersUAPIL2CPPARM64')
    {
        BuildUAPSupportConfig($root, "-sPLATFORM=uap_arm64 -sCONFIG=debug -sSCRIPTING_BACKEND=il2cpp $jamArgs MetroSupport");
        BuildUAPSupportConfig($root, "-sPLATFORM=uap_arm64 -sCONFIG=release -sSCRIPTING_BACKEND=il2cpp $jamArgs MetroSupport");
        BuildUAPSupportConfig($root, "-sPLATFORM=uap_arm64 -sCONFIG=master -sSCRIPTING_BACKEND=il2cpp $jamArgs MetroSupport");
    }
    elsif ($options->{codegen} eq 'devPlayersUAP' || $options->{codegen} eq 'abvsubsetUAP')
    {
        BuildUAPSupportConfig($root, "-sPLATFORM=uap_x64 -sCONFIG=debug -sSCRIPTING_BACKEND=dotnet $jamArgs MetroSupport");
        BuildUAPSupportConfig($root, "-sPLATFORM=uap_x64 -sCONFIG=debug -sSCRIPTING_BACKEND=il2cpp $jamArgs MetroSupport");
    }
    elsif ($options->{codegen} eq 'devPlayersUAPIL2CPP')
    {
        BuildUAPSupportConfig($root, "-sPLATFORM=uap_x64 -sCONFIG=debug -sSCRIPTING_BACKEND=il2cpp $jamArgs MetroSupport");
    }
    else
    {
        # This path is chosen when people try to build MetroSupport via build.pl
        if (IsWindows8OrNewer())
        {
            BuildUAPSupportConfig($root, "-sPLATFORM=uap_x64 -sCONFIG=" . $options->{codegen} . " -sSCRIPTING_BACKEND=dotnet $jamArgs MetroSupport");
        }
    }

    my @pdbs = ();
    find(
        {
            wanted => sub { /\.pdb$/ && push @pdbs, $File::Find::name; }
        },
        $targetPath
    );
    AddSourceServerToPDB($root, \@pdbs);

    if ($options->{buildAutomation})
    {
        JamRaw($root, 'MetroAutomation');
    }

    ClearUnusedFiles($root . "/build/MetroSupport");
    SignBinaries($root . "/build/MetroSupport");
    PackageAndSendResultsToBuildServer($targetPath, "MetroSupport");
}

1;
