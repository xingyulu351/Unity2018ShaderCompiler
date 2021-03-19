package SymbolFiles;

use strict;
use warnings;
use Carp qw(croak carp);
use File::Basename;
use File::Path qw(mkpath rmtree);
use File::Spec;
use File::chdir;
use File::Find;
use lib File::Spec->rel2abs(dirname($0) . "/..");
use Tools qw (UCopy ProgressMessage);
use lib File::Spec->rel2abs(dirname($0) . "/../../External/Perl/lib");
use OTEE::BuildUtils qw(svnGetRevision);

# SymbolFiles: gathers the pdb files from all the windows build targets and stores
# them for later use (for debugging).

my @foundPdbFiles = ();

sub Register
{
    Main::RegisterTarget(
        "SymbolFiles",
        {
            windowsbuildfunction => \&SymbolFiles,
            dependencies => ["windowseditor"]
        }
    );
}

# These files need to be copied and renamed so not to conflict with files which would otherwise have the same name
my %win32PdbFiles = (
    "External/Mono/builds/embedruntimes/win32/mono.pdb" => "win32",
    "External/Mono/builds/embedruntimes/win32/mono.dll" => "win32",
    "External/MonoBleedingEdge/builds/embedruntimes/win32/mono-2.0-bdwgc.dll" => "win32",
    "External/MonoBleedingEdge/builds/embedruntimes/win32/mono-2.0-bdwgc.pdb" => "win32",
    "build/WindowsStandaloneSupport/Variations/win32_nondevelopment_mono/WindowsPlayer.exe" => "win32/nondevelopment_mono",
    "build/WindowsStandaloneSupport/Variations/win32_nondevelopment_mono/UnityPlayer.dll" => "win32/nondevelopment_mono",
    "build/WindowsStandaloneSupport/Variations/win32_development_mono/WindowsPlayer.exe" => "win32/development_mono",
    "build/WindowsStandaloneSupport/Variations/win32_development_mono/UnityPlayer.dll" => "win32/development_mono",
    "build/WindowsStandaloneSupport/Variations/win32_nondevelopment_il2cpp/WindowsPlayer.exe" => "win32/nondevelopment_il2cpp",
    "build/WindowsStandaloneSupport/Variations/win32_nondevelopment_il2cpp/UnityPlayer.dll" => "win32/nondevelopment_il2cpp",
    "build/WindowsStandaloneSupport/Variations/win32_development_il2cpp/WindowsPlayer.exe" => "win32/development_il2cpp",
    "build/WindowsStandaloneSupport/Variations/win32_development_il2cpp/UnityPlayer.dll" => "win32/development_il2cpp",
);
my %win64PdbFiles = (
    "External/Mono/builds/embedruntimes/win64/mono.pdb" => "win64",
    "External/Mono/builds/embedruntimes/win64/mono.dll" => "win64",
    "External/MonoBleedingEdge/builds/embedruntimes/win64/mono-2.0-bdwgc.dll" => "win64",
    "External/MonoBleedingEdge/builds/embedruntimes/win64/mono-2.0-bdwgc.pdb" => "win64",

    # FIXME: we need to move the win64 build
    "build/WindowsEditor/Unity.exe" => "win64",
    "build/WindowsEditor/Data/Tools/JobProcess.exe" => "win64",
    "build/WindowsEditor/Data/Tools/UnityShaderCompiler.exe" => "win64",
    "build/WindowsEditor/Data/Tools/UnityYAMLMerge.exe" => "win64",
    "build/WindowsStandaloneSupport/Variations/win64_nondevelopment_mono/WindowsPlayer.exe" => "win64/nondevelopment_mono",
    "build/WindowsStandaloneSupport/Variations/win64_nondevelopment_mono/UnityPlayer.dll" => "win64/nondevelopment_mono",
    "build/WindowsStandaloneSupport/Variations/win64_development_mono/WindowsPlayer.exe" => "win64/development_mono",
    "build/WindowsStandaloneSupport/Variations/win64_development_mono/UnityPlayer.dll" => "win64/development_mono",
    "build/WindowsStandaloneSupport/Variations/win64_nondevelopment_il2cpp/WindowsPlayer.exe" => "win64/nondevelopment_il2cpp",
    "build/WindowsStandaloneSupport/Variations/win64_nondevelopment_il2cpp/UnityPlayer.dll" => "win64/nondevelopment_il2cpp",
    "build/WindowsStandaloneSupport/Variations/win64_development_il2cpp/WindowsPlayer.exe" => "win64/development_il2cpp",
    "build/WindowsStandaloneSupport/Variations/win64_development_il2cpp/UnityPlayer.dll" => "win64/development_il2cpp",
);

# If these are not in there then something has gone wrong and an error is triggered.

my @knownPdbFiles = ();
{
    @knownPdbFiles = (
        "Unity_x64_mono.pdb",

        "WindowsPlayer_Release_mono_x86.pdb",
        "WindowsPlayer_Master_mono_x86.pdb",
        "WindowsPlayer_Release_mono_x64.pdb",
        "WindowsPlayer_Master_mono_x64.pdb",
        "WindowsPlayer_Release_il2cpp_x86.pdb",
        "WindowsPlayer_Master_il2cpp_x86.pdb",
        "WindowsPlayer_Release_il2cpp_x64.pdb",
        "WindowsPlayer_Master_il2cpp_x64.pdb",

        "UnityPlayer_Win64_development_mono_x64.pdb",
        "UnityPlayer_Win32_development_mono_x86.pdb",
        "UnityPlayer_Win64_mono_x64.pdb",
        "UnityPlayer_Win32_mono_x86.pdb",
        "UnityPlayer_Win64_development_il2cpp_x64.pdb",
        "UnityPlayer_Win32_development_il2cpp_x86.pdb",
        "UnityPlayer_Win64_il2cpp_x64.pdb",
        "UnityPlayer_Win32_il2cpp_x86.pdb",

        "UnityPlayer_UAP_x86_debug_dotnet.pdb",
        "UnityPlayer_UAP_x86_release_dotnet.pdb",
        "UnityPlayer_UAP_x86_master_dotnet.pdb",
        "UnityPlayer_UAP_x64_debug_dotnet.pdb",
        "UnityPlayer_UAP_x64_release_dotnet.pdb",
        "UnityPlayer_UAP_x64_master_dotnet.pdb",
        "UnityPlayer_UAP_arm_debug_dotnet.pdb",
        "UnityPlayer_UAP_arm_release_dotnet.pdb",
        "UnityPlayer_UAP_arm_master_dotnet.pdb",

        "UnityPlayer_UAP_x86_debug_il2cpp.pdb",
        "UnityPlayer_UAP_x86_release_il2cpp.pdb",
        "UnityPlayer_UAP_x86_master_il2cpp.pdb",
        "UnityPlayer_UAP_x64_debug_il2cpp.pdb",
        "UnityPlayer_UAP_x64_release_il2cpp.pdb",
        "UnityPlayer_UAP_x64_master_il2cpp.pdb",
        "UnityPlayer_UAP_arm_debug_il2cpp.pdb",
        "UnityPlayer_UAP_arm_release_il2cpp.pdb",
        "UnityPlayer_UAP_arm_master_il2cpp.pdb",
    );
}

my @excludePdbFiles = ("vc100.pdb");

sub DestinationPath
{
    my ($src_file, $pdb_path, $subdir) = @_;

    my $category;
    my $dest_filename = basename($src_file);

    if ($src_file =~ /mono.*\.pdb$/)
    {
        # mono.pdb is a special case, as it's unstripped but open source,
        # we want it go to both private and public destination
        $category = "all";
    }
    elsif ($src_file =~ /_s\.pdb$/)
    {
        # stripped PDB file
        $category = "public";

        # remove the '_s' part from the file name, e.g. foo_s.pdb => foo.pdb
        $dest_filename = substr($dest_filename, 0, -6) . ".pdb";
    }
    elsif ($src_file =~ /\.pdb$/)
    {
        # unstripped PDB file
        $category = "private";
    }
    else
    {
        $category = "all";
    }
    return File::Spec->join($pdb_path, $category, $subdir, $dest_filename);
}

sub CopyToDestDir
{
    my ($src_path, $pdb_path, $subdir) = @_;
    my $dest_path = DestinationPath($src_path, $pdb_path, $subdir);

    ProgressMessage("Adding $src_path -> $dest_path");
    UCopy($src_path, $dest_path);
}

sub WriteManifest
{
    my ($pdb_path) = @_;
    my $manifest_path = File::Spec->join($pdb_path, "manifest");

    ProgressMessage("Adding archive manifest at $manifest_path");

    open(MANIFEST, ">$manifest_path");
    print(MANIFEST '{ "version": 1 }');
    close(MANIFEST);
}

sub SymbolFiles
{
    my ($root, $targetPath, $options) = @_;
    my $pdbPath = "$root/build/symbols";
    mkpath("$pdbPath");

    ProgressMessage("Collecting PDB files");

    # Grab the special files first
    for my $sourceFile (keys %win32PdbFiles)
    {
        CopyToDestDir("$root/$sourceFile", $pdbPath, $win32PdbFiles{$sourceFile});
    }
    for my $sourceFile (keys %win64PdbFiles)
    {
        CopyToDestDir("$root/$sourceFile", $pdbPath, $win64PdbFiles{$sourceFile});
    }

    # Grab all pdb files found in the build directories
    find(\&wanted, "$root/build");
    my $filter = join(" ", values %win32PdbFiles);
    for my $file (@foundPdbFiles)
    {
        # Bail if its one of the files we don't want
        my $filename = basename($file, ".pdb");
        my @match = grep(/^$filename/i, @excludePdbFiles);
        if (@match)
        {
            next;
        }

        my $cleanedFileName = File::Spec->abs2rel($file, "$root/build");
        $cleanedFileName =~ s/\\/\//g;
        if ($filter =~ /$cleanedFileName/ || $file =~ /temp/) { next; }

        if ($file =~ /_x86/)
        {
            CopyToDestDir($file, $pdbPath, "win32");
        }
        elsif ($file =~ /_x64/)
        {
            CopyToDestDir($file, $pdbPath, "win64");
        }
        else
        {
            # Skip any file in the target path
            if (!($file =~ /$pdbPath/))
            {
                CopyToDestDir($file, $pdbPath, "");
            }
        }
    }

    # Verify all the files in the knownPdbFiles array have been copied over
    my %copiedFiles = ();
    local $CWD = $pdbPath;
    my $missingFiles = 0;

    @foundPdbFiles = ();
    find(\&pdbCheck, $pdbPath);
    for my $file (@foundPdbFiles)
    {
        $copiedFiles{ basename($file) } = 1;
    }
    for my $knownFile (@knownPdbFiles)
    {
        if (!exists($copiedFiles{ basename($knownFile) }))
        {
            $missingFiles++;
            ProgressMessage("missing pdb: $knownFile");
        }
    }
    if ($missingFiles > 0) { croak("Missing needed pdbs, exiting before archiving.") }

    WriteManifest($pdbPath);

    # Put all symbols files into a ZIP archive
    ProgressMessage("Archiving all PDB files (internal full)");
    system("$root/External/7z/win32/7za.exe", 'a', '-r', '-tzip', "$targetPath/misc-pdb-internal.zip", "$pdbPath/*") && croak('FAILED to create pdb archive');
}

sub wanted
{
    # Skip anything in the symbols path that's already there and mono dlls
    /\.pdb$/ && !/^mono/ && !($File::Find::dir =~ /symbols/) && push @foundPdbFiles, $File::Find::name;
}

sub pdbCheck
{
    /\.pdb$/ && push @foundPdbFiles, $File::Find::name;
}

1;
