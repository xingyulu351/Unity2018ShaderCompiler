package TargetSupportInstaller;

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
use OTEE::BuildUtils qw(svnGetRevision);
use Tools qw (SetupPlaybackEngines UCopy ProgressMessage ReplaceText ReadFile Jam);
use PrepareWorkingCopy qw (PrepareExternalDependency);
use WinTools qw (SignFile);
use MacTools qw (BuildPackage BuildMacProductInstaller BuildInstallerFromPackages SignPackage BuildAndPatchComponentPlist SetCustomFileIcon);
use Data::Dumper;
use if $^O eq 'MSWin32', 'Win32::Exe';
use MacInstallerLicensePlugin qw(BuildMacInstallerLicensePlugin);

# Nice target name
my $niceNames = {
    "Switch" => "Nintendo Switch",
    "UAPSupport_IL2CPP" => "UWP IL2CPP",
    "PS4Player" => "Playstation 4",
    "XboxOnePlayer" => "Xbox One",
    "WebGLSupport" => "WebGL",
    "LinuxStandaloneSupport" => "Linux",
    "iOSSupport" => "iOS",
    "AppleTVSupport" => "AppleTV",
    "AndroidPlayer" => "Android",
    "MetroSupport" => "UWP .NET",
    "MacStandaloneSupport" => "Mac Mono",
    "MacStandaloneSupportIL2CPP" => "Mac IL2CPP",
    "WindowsStandaloneSupport" => "Windows Mono",
    "WindowsStandaloneSupportIL2CPP" => "Windows IL2CPP",
    "Facebook" => "Facebook Games",
    "VuforiaSupport" => "Vuforia AR",
};

my $assemblyPath;

sub Register
{
    Main::RegisterTarget(
        "TargetSupportInstaller",
        {
            windowsbuildfunction => \&BuildTargetSupportInstallerWin,
            macbuildfunction => \&BuildTargetSupportInstallerMac,
            linuxbuildfunction => \&BuildTargetSupportInstallerLinux,
            options => {
                "playbackengines" => ["Switch", "PS4Player", "XboxOnePlayer"],
                "customInstallerName" => ["Consoles", ""],
                "codegen" => ["debug", "release"],
                "incremental" => [0, 1],
                "platform" => []
            }
        }
    );
}

sub GetPlaybackEnginePath
{
    my ($playbackEngine) = @_;

    if ($playbackEngine eq "UAPSupport_IL2CPP")
    {
        return "MetroSupport";
    }

    if ($playbackEngine eq "MacStandaloneSupportIL2CPP")
    {
        return "MacStandaloneSupport";
    }

    if ($playbackEngine eq "WindowsStandaloneSupportIL2CPP")
    {
        return "WindowsStandaloneSupport";
    }

    return $playbackEngine;
}

sub BuildTargetSupportInstallerCommon
{
    my ($root, $targetPath, $options, $finished_callback) = @_;
    my $playbackEngine = $options->{playbackengines}[0];
    my $playbackEnginePath = GetPlaybackEnginePath($playbackEngine);

    if (!defined($niceNames->{$playbackEngine}))
    {
        die(      "Nice name for the playback engine '$playbackEngine' is not found. Available names: "
                . Dumper($niceNames)
                . "\nPlease note that the playback engine names are case sensitive.\n");
    }

    local $CWD = "$root/Tools/Installers/WindowsTargetSupport";

    # Bound editor version
    my $editorVersion = $ENV{'OVERRIDE_EDITOR_VERSION'} || "";
    if ($editorVersion eq "")
    {
        if ($^O eq 'MSWin32')
        {
            my $exe = Win32::Exe->new("$root/build/WindowsEditor/Unity.exe");
            my $ver = $exe->get_version_info();

            $editorVersion = $ver->{"FileVersion"};
        }
        elsif ($^O eq 'darwin')
        {
            my $info = "$root/build/MacEditor/Unity.app/Contents/Info.plist";
            my $plist = ReadFile($info) or die("Failed to read $info\n");
            if ($plist =~ /<key>CFBundleVersion<\/key>\s*<string>([^>]+)<\/string>/)
            {
                $editorVersion = $1;
            }
        }
        print("Editor build version: $editorVersion\n");
    }
    else
    {
        print("Override bound editor build version: $editorVersion\n");
    }

    # Full build version
    my $fullVersion = "";
    my $fullVersionNice = "";
    if (open(my $ver_file, "<", "$root/build/$playbackEnginePath/version.txt"))
    {
        $fullVersion = readline($ver_file);
        close($ver_file);

        chomp($fullVersion);
        print("Found target support build version: $fullVersion\n");

        $fullVersionNice = "($fullVersion)";
    }

    # Final file name
    my $targetNiceName = $niceNames->{$playbackEngine};
    my $targetFinalFileName = $targetNiceName;
    $targetFinalFileName =~ s/ /-/g;
    $targetFinalFileName = "UnitySetup-$targetFinalFileName-Support-for-Editor-$BuildConfig::unityVersion"; # note: file extension is added by OS specific build method

    PrepareExternalDependency("External/Installers/NSIS");

    $finished_callback->($root, $targetPath, $options, $targetFinalFileName, $editorVersion, $fullVersion);
}

sub BuildActualWindowsInstaller
{
    my ($root, $targetPath, $options, $targetFinalFileName, $editorVersion, $fullVersion) = @_;
    my ($ver_version, $ver_major, $ver_minor) = $BuildConfig::unityVersion =~ /^(\d+)\.(\d+)\.(\d+)/;
    my $custom_name = $options->{customInstallerName} eq "" ? "" : " $options->{customInstallerName}";
    my $playbackEngine = $options->{playbackengines}[0];
    my $playbackEnginePath = GetPlaybackEnginePath($playbackEngine);
    my $targetNiceName = $niceNames->{$playbackEngine};
    my $targetName = $playbackEngine;

    DeleteUnneededFiles($root, $targetName);

    # For alpha or beta releases, include version number in install path
    my $install_folder = "Unity${custom_name}";
    $install_folder .= " $BuildConfig::unityVersion" if ($BuildConfig::unityVersion =~ /\d+.\d+.\d+[ab]\d+/);
    ReplaceText(
        'UnityPathsTemplate.nsh',
        'UnityPaths.nsh',
        (
            REPLACE_VERSION_STRING => $BuildConfig::unityVersion,
            REPLACE_VERSION_WIN => "${ver_version}.${ver_major}.${ver_minor}.0",
            REPLACE_VERSION_NICE => $BuildConfig::unityVersion,
            REPLACE_CUSTOM_NAME => ${custom_name},
            REPLACE_COMPANY_NAME => $BuildConfig::company,
            REPLACE_COPYRIGHT => $BuildConfig::copyright,
            REPLACE_ENGINE_FOLDER => $playbackEnginePath,
            REPLACE_ENGINE_NICE_NAME => $targetNiceName,
            REPLACE_FINAL_FILE_NAME => $targetFinalFileName . '.exe',
            REPLACE_ENGINE_BACKGROUND_IMG => "..\\WindowsEditor\\Background.bmp",    #ToDo: add fancy console logos
            REPLACE_TARGET_SUPPORT_VERSION => $fullVersion,
            REPLACE_BOUND_EDITOR_VERSION => $editorVersion,
            REPLACE_INSTALL_FOLDER => $install_folder,
        )
    );

    # Include a target (message only)
    my $engineNameCorrectCasing = basename($playbackEngine);
    print("Adding $engineNameCorrectCasing\n");

    # Make & copy doccombiner
    if (-d "$root/build/UserDocumentation$targetName/Documentation")
    {
        ProgressMessage("Making DocCombiner");
        my $platform = ($options->{platform} or "win64");
        Jam($root, 'DocCombiner', $options->{codegen}, $platform, $options->{incremental});
        UCopy("$root/build/WindowsDocCombiner/DocCombiner.exe", "$root/build/UserDocumentation$targetName/Documentation/");
    }
    else
    {
        print "$root/build/UserDocumentation$targetName/Documentation doesn't exist\n";
    }

    #build the actual editor installer
    my $finalPath = "$targetPath/$targetFinalFileName";
    print("Creating final installer at path: $finalPath.exe\n");

    system("$root/External/Installers/NSIS/builds/makensis.exe", "/V2", "SetupTargetSupport.nsi") eq 0 or croak("Failing creating final installer");
    if (not -e "$finalPath.exe") { croak("Creating installer returned succesfully, however the produced installer is not found at $finalPath.exe\n"); }

    print("Signing final installer\n");
    SignFile("$finalPath.exe");
}

sub BuildTargetSupportInstallerWin
{
    BuildTargetSupportInstallerCommon(@_, \&BuildActualWindowsInstaller);
}

sub BuildActualMacInstaller
{
    my ($root, $targetPath, $options, $targetFinalFileName, $editorVersion) = @_;

    my $finalPath = "$targetPath/$targetFinalFileName";
    print("Creating final installer at path: $finalPath.pkg\n");

    my $playbackEngine = $options->{playbackengines}[0];
    my $playbackEnginePath = GetPlaybackEnginePath($playbackEngine);
    my $targetName = $playbackEngine;

    DeleteUnneededFiles($root, $targetName);

    rmtree("$assemblyPath/InstallerSources/MacTargetSupport");
    UCopy("$root/Tools/Installers/MacTargetSupport", "$assemblyPath/InstallerSources/MacTargetSupport");

    my $pluginsPath = BuildMacInstallerLicensePlugin($root, $assemblyPath);

    # assemble the installer image
    rmtree("$assemblyPath/TargetSupportInstaller");
    mkpath("$assemblyPath/TargetSupportInstaller");

    # copy player
    UCopy("$root/build/$playbackEnginePath", "$assemblyPath/TargetSupportInstaller/$playbackEnginePath");

    if (-d "$root/build/$playbackEnginePath/$playbackEnginePath")
    {
        warn("Fishy folder: $root/build/$playbackEnginePath/$playbackEnginePath\n");
    }

    if (!-e "$root/build/$playbackEnginePath/ivy.xml")
    {
        die "$root/build/$playbackEnginePath/ivy.xml is missing, can't build this TargetSupportInstaller\n";
    }

    # copy docs
    if (-d "$root/build/UserDocumentation$targetName/Documentation")
    {
        UCopy("$root/build/UserDocumentation$targetName/Documentation", "$assemblyPath/TargetSupportInstaller/$targetName/Documentation");
        ProgressMessage("Making DocCombiner");
        my $platform = ($options->{platform} or "macosx64");
        Jam($root, 'DocCombiner', $options->{codegen}, $platform, $options->{incremental});
        UCopy("$root/build/MacDocCombiner", "$assemblyPath/TargetSupportInstaller/$targetName/Documentation/");
        system("chmod +x $assemblyPath/TargetSupportInstaller/$targetName/Documentation/DocCombiner");
    }
    else
    {
        print "$root/build/UserDocumentation$targetName/Documentation doesn't exist\n";
    }

    my $cont;
    my $fh;

    # patch the template distribution.plist with real data
    my $distplist = "$assemblyPath/InstallerSources/MacTargetSupport/distribution.plist";
    $cont = ReadFile($distplist) or die "Failed to read $distplist\n";
    $cont =~ s/REPLACE_PACKAGE_NAME/$targetName/g;
    $cont =~ s/REPLACE_PACKAGE_ID/$targetName/g;
    open($fh, ">$distplist") or die("Failed to write $distplist\n");
    print $fh ($cont);
    close($fh);

    my $preinstall = "$assemblyPath/InstallerSources/MacTargetSupport/Scripts/preinstall";
    $cont = ReadFile($preinstall) or die "Failed to read $preinstall\n";
    $cont =~ s/REPLACE_PACKAGE_FOLDER/PlaybackEngines\/$playbackEnginePath/g;
    $cont =~ s/REPLACE_UNITY_EDITOR_VERSION/$editorVersion/g;
    open($fh, ">$preinstall") or die("Failed to write $preinstall\n");
    print $fh ($cont);
    close($fh);

    my $postinstall = "$assemblyPath/InstallerSources/MacTargetSupport/Scripts/postinstall";
    $cont = ReadFile($postinstall) or die "Failed to read $postinstall\n";
    $cont =~ s/REPLACE_PACKAGE_FOLDER/PlaybackEngines\/$playbackEnginePath/g;
    open($fh, ">$postinstall") or die("Failed to write $postinstall\n");
    print $fh ($cont);
    close($fh);

    UCopy("$root/Tools/Licenses/License.rtf", "$assemblyPath/InstallerSources/MacTargetSupport/Resources/License.rtf");
    UCopy("$root/Tools/Installers/InstallerAssets/background.tiff", "$assemblyPath/InstallerSources/MacTargetSupport/Resources/");

    my $comp_plist = "$assemblyPath/InstallerSources/MacTargetSupport/component.plist";
    BuildAndPatchComponentPlist($targetPath, $comp_plist);

    my $packageName = "$assemblyPath/TargetSupport.pkg.tmp";
    my $sourceDir;
    my $packageTargetPath;
    my $scriptsPath = "$assemblyPath/InstallerSources/MacTargetSupport/Scripts";
    my $identifier = "com.unity3d.$targetName";

    if ($targetName eq "iOSSupport")
    {
        $sourceDir = "$assemblyPath/TargetSupportInstaller";
        $packageTargetPath = "/Applications/Unity/PlaybackEngines";
    }
    else
    {
        $sourceDir = "$assemblyPath/TargetSupportInstaller/$playbackEnginePath";
        if ($targetName eq "MacStandaloneSupportIL2CPP")
        {
            $packageTargetPath = "/Applications/Unity/Unity.app/Contents/PlaybackEngines/$playbackEnginePath";
        }
        else
        {
            $packageTargetPath = "/Applications/Unity/PlaybackEngines/$playbackEnginePath";
        }
    }

    # run pkgbuild
    BuildPackage($packageName, $sourceDir, $packageTargetPath, $scriptsPath, $identifier, $comp_plist);

    my @packages = ($packageName);

    BuildMacProductInstaller(
        targetPath => "$finalPath.pkg",
        sourceFiles => "$assemblyPath/InstallerSources/MacTargetSupport",
        packageList => \@packages,
        pluginsPath => $pluginsPath
    );
    SetCustomFileIcon("$finalPath.pkg", "$root/Tools/Installers/InstallerAssets/installer.icns");
}

sub BuildTargetSupportInstallerMac
{
    my ($root, $targetPath, $options) = @_;
    $assemblyPath = "$root/build/temp";
    BuildTargetSupportInstallerCommon($root, $targetPath, $options, \&BuildActualMacInstaller);
}

sub BuildActualLinuxInstaller
{
    my ($root, $targetPath, $options, $targetFinalFileName) = @_;

    my $finalPath = "$targetPath/$targetFinalFileName";
    print("Creating final installer at path: $finalPath.tar.xz\n");

    my $playbackEngine = $options->{playbackengines}[0];
    my $targetName = $playbackEngine;

    # assemble the installer image
    my $deployPath = "$assemblyPath/TargetSupportInstaller/Editor/Data/PlaybackEngines";
    rmtree("$assemblyPath/TargetSupportInstaller");
    mkpath($deployPath);

    # copy player
    UCopy("$root/build/$targetName", "$deployPath/$targetName");

    if (-d "$root/build/$targetName/$targetName")
    {
        warn("Fishy folder: $root/build/$targetName/$targetName\n");
    }

    if (!-e "$root/build/$targetName/ivy.xml")
    {
        die "$root/build/$targetName/ivy.xml is missing, can't build this TargetSupportInstaller\n";
    }

    # copy docs
    if (-d "$root/build/UserDocumentation$targetName/Documentation")
    {
        UCopy("$root/build/UserDocumentation$targetName/Documentation", "$deployPath/$targetName/Documentation");
        ProgressMessage("Making DocCombiner");
        my $platform = ($options->{platform} or "linux64");
        Jam($root, 'DocCombiner', $options->{codegen}, $platform, $options->{incremental});
        UCopy("$root/build/LinuxDocCombiner", "$deployPath/$targetName/Documentation/");
        system("chmod +x $deployPath/$targetName/Documentation/DocCombiner");
    }
    else
    {
        print "$root/build/UserDocumentation$targetName/Documentation doesn't exist\n";
    }

    system('tar', 'cJf', "$finalPath.tar.xz", '--directory', "$assemblyPath/TargetSupportInstaller", '.')
        && croak "Error building target support installer from $assemblyPath/TargetSupportInstaller/$targetName";
}

sub BuildTargetSupportInstallerLinux
{
    my ($root, $targetPath, $options) = @_;
    $assemblyPath = "$root/build/temp";
    BuildTargetSupportInstallerCommon($root, $targetPath, $options, \&BuildActualLinuxInstaller);
}

sub FindReplaceUnstrippedPDB
{
    my $strippedPDB = $File::Find::name;
    if ($strippedPDB =~ /_s.pdb$/)
    {
        my $unstrippedPDB = substr($strippedPDB, 0, -6) . ".pdb";
        if (-e $unstrippedPDB)
        {
            unlink($unstrippedPDB) or die("Failed to delete $unstrippedPDB\n");
        }
        rename($strippedPDB, $unstrippedPDB) or die("Failed to rename $strippedPDB to $unstrippedPDB\n");
        print("Replaced/Renamed $strippedPDB to $unstrippedPDB\n");
    }
}

sub DeleteAllUnstrippedPdbsRecursive
{
    my ($pdbFolder) = @_;
    find(\&FindReplaceUnstrippedPDB, $pdbFolder);
}

sub DeleteUnstrippedPdb
{
    my ($pdbFolder, $pdbName, $strippedPdbName) = @_;

    # Delete non-stripped PDBs (they go into our symbol server) and rename stripped ones to be named as non-stripped PDBs.
    if (-e "$pdbFolder/$pdbName" && -e "$pdbFolder/$strippedPdbName")
    {
        print("deleting $pdbFolder/$pdbName\n");
        unlink("$pdbFolder/$pdbName") or die("Failed to delete $pdbFolder/$pdbName\n");
        rename("$pdbFolder/$strippedPdbName", "$pdbFolder/$pdbName") or die("Failed to rename $pdbFolder/$strippedPdbName to $pdbFolder/$pdbName\n");
    }
}

sub DeleteUnstrippedWindowsStandalonePdb
{
    my ($pdbFolder, $fileBaseName, $arch, $scriptingBackend) = @_;

    my $pdbName = "${fileBaseName}_${scriptingBackend}_${arch}.pdb";
    my $strippedPdbName = "${fileBaseName}_${scriptingBackend}_${arch}_s.pdb";

    DeleteUnstrippedPdb($pdbFolder, $pdbName, $strippedPdbName);
}

sub DeleteUnneededWindowsStandaloneFiles
{
    my ($root, $winBitnessName, $arch, $config, $scriptingBackend) = @_;

    my $pdbFolder = "$root/build/windowsstandalonesupport/Variations/${winBitnessName}_${config}_${scriptingBackend}";
    my $unityPlayerPdbSuffix = $config eq "nondevelopment" ? "" : "_development";
    my $windowsPlayerConfig = $config eq "nondevelopment" ? "Master" : "Release";

    DeleteUnstrippedWindowsStandalonePdb($pdbFolder, "UnityPlayer_${winBitnessName}${unityPlayerPdbSuffix}", $arch, $scriptingBackend);
    DeleteUnstrippedWindowsStandalonePdb($pdbFolder, "WindowsPlayer_${windowsPlayerConfig}", $arch, $scriptingBackend);
}

sub DeleteUnneededFiles
{
    my ($root, $targetName) = @_;

    if ($targetName eq "MetroSupport")
    {
        DeleteAllUnstrippedPdbsRecursive("$root\\build\\MetroSupport\\Players");
    }
    elsif ($targetName eq "UAPSupport_IL2CPP")
    {
        DeleteAllUnstrippedPdbsRecursive("$root\\build\\MetroSupport\\Players");
    }
    elsif ($targetName eq "WindowsStandaloneSupport")
    {
        DeleteUnneededWindowsStandaloneFiles($root, "Win32", "x86", "development", "mono");
        DeleteUnneededWindowsStandaloneFiles($root, "Win32", "x86", "nondevelopment", "mono");
        DeleteUnneededWindowsStandaloneFiles($root, "Win64", "x64", "development", "mono");
        DeleteUnneededWindowsStandaloneFiles($root, "Win64", "x64", "nondevelopment", "mono");
    }
    elsif ($targetName eq "WindowsStandaloneSupportIL2CPP")
    {
        DeleteUnneededWindowsStandaloneFiles($root, "Win32", "x86", "development", "il2cpp");
        DeleteUnneededWindowsStandaloneFiles($root, "Win32", "x86", "nondevelopment", "il2cpp");
        DeleteUnneededWindowsStandaloneFiles($root, "Win64", "x64", "development", "il2cpp");
        DeleteUnneededWindowsStandaloneFiles($root, "Win64", "x64", "nondevelopment", "il2cpp");
    }
}

1;
