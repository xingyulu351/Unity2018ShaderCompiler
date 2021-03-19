package WindowsEditorInstaller;

use File::Basename qw(dirname basename fileparse);
use File::Copy;
use File::Spec;
use File::Path;
use File::Glob;
use File::Find;
use Carp qw(croak carp);
use File::chdir;
use OTEE::BuildUtils qw(svnGetRevision);
use Tools qw (SetupPlaybackEngines ProgressMessage ReplaceText AmRunningOnBuildServer UCopy Jam);
use WinTools qw (SignFile);
use Number::Bytes::Human qw(format_bytes);
use UserDocumentation;
use Digest::MD5;
use strict;
use warnings;

use BuildConfig;
our $unityVersion = $BuildConfig::unityVersion;

require Exporter;
our @ISA = qw(Exporter);

sub Register
{
    Main::RegisterTarget(
        "WindowsEditorInstallerSet",
        {
            windowsbuildfunction => \&BuildWindowsEditorInstallerSet,
            dependencies => [],
        }
    );
    Main::RegisterTarget(
        "Windows32EditorInstaller",
        {
            windowsbuildfunction => \&BuildWindows32EditorInstaller,
            dependencies => ["WindowsEditor", "WindowsStandaloneSupport",],
            options => { "playbackengines" => ["WindowsStandaloneSupport"], "customInstallerName" => ["Consoles", ""] }
        }
    );
    Main::RegisterTarget(
        "Windows64EditorInstaller",
        {
            windowsbuildfunction => \&BuildWindows64EditorInstaller,
            dependencies => ["WindowsEditor", "WindowsStandaloneSupport",],
            options => { "playbackengines" => ["WindowsStandaloneSupport"], "customInstallerName" => ["Consoles", ""] }
        }
    );
    Main::RegisterTarget(
        "WindowsDocumentationInstaller",
        {
            windowsbuildfunction => \&BuildWindowsDocumentationInstaller,
            options => {
                "codegen" => ["debug", "release"],
                "incremental" => [0, 1],
                "platform" => []
            },
            dependencies => ["UserDocumentation",],
        }
    );
    Main::RegisterTarget(
        "WindowsMonoDevelopInstaller",
        {
            windowsbuildfunction => \&BuildWindowsMonoDevelopInstaller,
            options => {
                "codegen" => ["debug", "release"],
                "incremental" => [0, 1],
                "platform" => []
            },
            dependencies => [],
        }
    );
    Main::RegisterTarget(
        "WindowsBootstrapper",
        {
            windowsbuildfunction => \&BuildWindowsBootstrapper,
            dependencies => [],
            options => {}
        }
    );
}

sub BuildWindowsEditorInstaller_shared
{
    my ($root, $targetPath, $options, $win64) = @_;

    WriteUnityPathsFile($options, $root);
    local $CWD = "$root/Tools/Installers/WindowsEditor";

    my $tempPath = "$root/build/temp";

    #Create a temporary installer, whose only purpose it is to generate the uninstaller when ran itself. Funky but works.

    # Monodoc generation requires building documentation tools, which corrupt the WindowsEditor folder.
    # Back it up.
    rename("$root/build/WindowsEditor", "$root/build/SavedWindowsEditor")
        or die("Failed to rename $root/build/WindowsEditor to $root/build/SavedWindowsEditor\n");

    UserDocumentation::BuildUserDocumentation_MonoDoc("$root/build/WindowsEditor");

    # copy all generated xmls
    UCopy("$root/build/WindowsEditor/Data/Managed/*.xml", "$root/build/SavedWindowsEditor/Data/Managed/");
    UCopy("$root/build/WindowsEditor/Data/Managed/UnityEngine/*.xml", "$root/build/SavedWindowsEditor/Data/Managed/UnityEngine/");

    # remove the monodoc's WindowsEditor
    rmtree("$root/build/WindowsEditor") or croak("Can't delete monodoc-generated WindowsEditor folder");

    # restore the saved version
    rename("$root/build/SavedWindowsEditor", "$root/build/WindowsEditor")
        or die("Failed to rename $root/build/SavedWindowsEditor to $root/build/WindowsEditor\n");

    # Gather requested playbackengines for inclusion in the installer (NSIS grabs everything here and places in $INSTDIR/Data/PlaybackEngines)
    SetupPlaybackEngines($root, $options, "$tempPath/PlaybackEngines");
    WriteUnityPdbPathsFile($options, $root, "FilelistInstall_pdb.nsh");

    ProgressMessage("Adding Plugin Interface Header Files");
    rmtree("$root/build/WindowsEditor/Data/PluginAPI");
    UCopy("$root/Runtime/PluginInterface/Headers", "$root/build/WindowsEditor/Data/PluginAPI");

    my $architectureDefine = ($win64 ? "/DARCHITECTURE=\"x64\"" : "/DARCHITECTURE=\"x86\"");

    ProgressMessage("Create a temporary installer, whose only purpose it is to generate the uninstaller when ran itself.");
    system("$root/External/Installers/NSIS/builds/makensis.exe", "/V3", $architectureDefine, "/DDO_UNINSTALLER", "SetupUnity.nsi") eq 0
        or croak("failed creating uninstalling creating installer");
    ProgressMessage("Running the uninstaller generating installer.");
    system("$tempPath/temp_UnityUnnstallerCreator.exe") or croak("Can't run temp_UnityUnnstallerCreator.exe");

    #Sign our executables.
    SignFile("$tempPath/Uninstall.exe");

    my $arch = $win64 ? "64" : "32";
    my $target = "$targetPath/UnitySetup$arch.exe";

    #build the actual editor installer
    ProgressMessage("Creating final installer at path: $target");
    system("$root/External/Installers/NSIS/builds/makensis.exe", "/V3", $architectureDefine, "SetupUnity.nsi") eq 0
        or croak("Failing creating final installer");
    if (not -e $target)
    {
        croak("Creating installer returned succesfully, however the produced installer is not found at $target\n");
    }

    ProgressMessage("Signing final installer");
    SignFile($target);
}

sub WriteUnityPathsFile
{
    my ($options, $root) = @_;

    my ($ver_version, $ver_major, $ver_minor) = $BuildConfig::unityVersion =~ /^(\d+)\.(\d+)\.(\d+)/;
    my $custom_name = "";
    if ($options->{customInstallerName})
    {
        $custom_name = " $options->{customInstallerName}";
    }

    # For alpha, beta, or experimental releases, include version number in install path
    my $install_folder = "Unity${custom_name}";
    $install_folder .= " $BuildConfig::unityVersion" if ($BuildConfig::unityVersion =~ /\d+.\d+.\d+[abx].*\d+/);

    local $CWD = "$root/Tools/Installers/WindowsEditor";
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
            REPLACE_INSTALL_FOLDER => $install_folder,
        )
    );

    my %svn_info = svnGetRevision($root);
    my $svn_revision = $svn_info{Revision};

    if (!$svn_revision)
    {
        die("svnGetRevision returned 0 hg revision, which is not an acceptable value\n");
    }

    open my $INI, ">> UnityPaths.nsh" or die "Failed to open UnityPaths.nsh for writing ini file locations";
    my $mirroridx = 1;
    foreach my $iniloc (@BuildConfig::bootstrapperIniLocations)
    {
        print $INI "\n!define IN_BOOTSTRAPPER_INI_URL$mirroridx \"$iniloc/$svn_revision/$BuildConfig::bootstrapperIniFileWin\"";
        $mirroridx++;
    }
    close $INI;
}

sub WriteUnityPdbPathsFile
{
    my ($options, $root, $filename) = @_;
    my $playbackEnginesFolder = "$root/build/temp/PlaybackEngines";

    open(my $fh, '>', $filename) or die "Could not open file '$filename' $!";
    find(
        sub
        {
            my $infile = $File::Find::name;

            # Find 32- or 64-bit "*_s.pdb" files which are not located within a "*nondevelopment*" path.
            # Update: This also should include pdb files from Metro/WP8 folder, for ex., UnityPlayer_Windows81_x86_master_s.pdb
            if ($infile =~ /_s\.pdb$/)
            {
                # SetOutPath
                my $outpath = $File::Find::dir;
                $outpath =~ s/\Q$playbackEnginesFolder//;    # remove first part of path
                $outpath =~ s/\//\\/g;                       # replace forward with backslashes
                $outpath =~ s/^\\//;                         # remove leading backslash
                print $fh "\nSetOutPath \"\$INSTDIR\\Editor\\Data\\PlaybackEngines\\$outpath\\\"\n";

                # File
                $infile =~ s/\//\\/g;
                my $outfile = $_;
                $outfile =~ s/_s\.pdb/\.pdb/;
                print $fh "File \"/oname=$outfile\" \"$infile\"\n";
                print("Include PDB: $infile -> $outfile\n");
            }
            elsif ($infile =~ /\.pdb$/)
            {
                print("Exclude PDB: $infile\n");
                unlink($infile) or die("Failed to delete $infile\n");
            }
        },
        $playbackEnginesFolder
    );
    close $fh;
}

sub BuildWindows32EditorInstaller
{
    my ($root, $targetPath, $options, $win64) = @_;
    BuildWindowsEditorInstaller_shared($root, $targetPath, $options);
}

sub BuildWindowsEditorInstallerSet
{
    my ($root, $targetPath, $options) = @_;

    # Generate the bootstrapper ini file
    my @modules = (
        {
            "name" => "Unity",
            "build" => "$root/build/Windows64EditorInstaller/UnitySetup64.exe",
            "description" => "Unity Editor for building your games",
            "url" => "Windows64EditorInstaller/UnitySetup64.exe",
            "mandatory" => "false",
            "install" => "true",
            "cmd" => '"{FILENAME}" -UI=reduced /D={INSTDIR}',
            "installedsize" => "5044713",
        },
        {
            "name" => "VisualStudio",
            "title" => "Microsoft Visual Studio Community 2017",
            "description" =>
                "Downloads and installs Microsoft Visual Studio Community 2017. By installing this you accept the license terms for Visual Studio.",
            "url" => "https://aka.ms/vs/15/release/vs_Community.exe",
            "url_32" => "https://aka.ms/vs/15/release/vs_Community.exe",
            "eulamessage" => "Please review and accept the license terms before downloading and installing Microsoft Visual Studio.",
            "eulaurl1" => "https://go.microsoft.com/fwlink/?LinkId=831111",
            "eulalabel1" => "Visual Studio 2017 Community License Terms",
            "size" => "1000000", # The VS 2017 RC installer size is 1 Mb (this is what we download), which in turn downloads and unfolds into something like 1.3 GB
            "size_h" => "1.0M",
            "installedsize" => "1300000",
            "filename" => "vs_Community.exe",
            "mandatory" => "false",
            "install" => "true",
            "cmd" =>
                '"{FILENAME}" --productId "Microsoft.VisualStudio.Product.Community" --add "Microsoft.VisualStudio.Workload.ManagedGame" --add "Microsoft.VisualStudio.Workload.NativeDesktop" --add "Microsoft.VisualStudio.Component.VC.Tools.x86.x64" --add "Microsoft.VisualStudio.Component.Windows10SDK.16299.Desktop" --campaign "Unity3d_Unity" --passive --norestart --wait',
            "use_vs_editor" => "true",
        },
        {
            "name" => "VisualStudioProfessionalUnityWorkload",
            "title" => "Unity Game Development for Microsoft Visual Studio Professional 2017",
            "description" =>
                "Unity Game Development for Microsoft Visual Studio Professional 2017. By installing this you accept the license terms for Visual Studio Tools for Unity.",
            "url" => "https://aka.ms/vs/15/release/vs_Professional.exe",
            "url_32" => "https://aka.ms/vs/15/release/vs_Professional.exe",
            "eulamessage" =>
                "Please review and accept the license terms before downloading and installing Unity Game Development for Microsoft Visual Studio Professional 2017.",
            "eulaurl1" => "https://go.microsoft.com/fwlink/?linkid=617019",
            "eulalabel1" => "Visual Studio Tools for Unity License Terms",
            "size" => "1000000",
            "size_h" => "1.0M",
            "installedsize" => "320000",
            "filename" => "vs_Professional.exe",
            "mandatory" => "false",
            "install" => "false",
            "cmd" =>
                '"{FILENAME}" --add "Microsoft.VisualStudio.Workload.ManagedGame" --add "Microsoft.VisualStudio.Workload.NativeDesktop" --add "Microsoft.VisualStudio.Component.VC.Tools.x86.x64" --add "Microsoft.VisualStudio.Component.Windows10SDK.16299.Desktop" --passive --norestart --wait',
            "use_vs_editor" => "true",
        },
        {
            "name" => "VisualStudioEnterpriseUnityWorkload",
            "title" => "Unity Game Development for Microsoft Visual Studio Enterprise 2017",
            "description" =>
                "Unity Game Development for Microsoft Visual Studio Enterprise 2017. By installing this you accept the license terms for Visual Studio Tools for Unity.",
            "url" => "https://aka.ms/vs/15/release/vs_Enterprise.exe",
            "url_32" => "https://aka.ms/vs/15/release/vs_Enterprise.exe",
            "eulamessage" =>
                "Please review and accept the license terms before downloading and installing Unity Game Development for Microsoft Visual Studio Enterprise 2017.",
            "eulaurl1" => "https://go.microsoft.com/fwlink/?LinkId=617019",
            "eulalabel1" => "Visual Studio Tools for Unity License Terms",
            "size" => "1000000",
            "size_h" => "1.0M",
            "installedsize" => "320000",
            "filename" => "vs_Enterprise.exe",
            "mandatory" => "false",
            "install" => "false",
            "cmd" =>
                '"{FILENAME}" --add "Microsoft.VisualStudio.Workload.ManagedGame" --add "Microsoft.VisualStudio.Workload.NativeDesktop" --add "Microsoft.VisualStudio.Component.VC.Tools.x86.x64" --add "Microsoft.VisualStudio.Component.Windows10SDK.16299.Desktop" --passive --norestart --wait',
            "use_vs_editor" => "true",
        },
        {
            "name" => "FacebookGameroom",
            "title" => "Facebook Gameroom",
            "description" => "Downloads and installs Facebook Gameroom.",
            "url" => "https://www.facebook.com/games/desktopapp/download_latest_installer?__mref=message_bubble",
            "filename" => "FacebookGamesArcade.msi",
            "size" => "33000",
            "size_h" => "33.0M",
            "installedsize" => "100000",
            "mandatory" => "false",
            "install" => "false",
            "cmd" => 'msiexec.exe /i "{FILENAME}" /qr LAUNCHEXE=FALSE',
            "hidden" => "true",
            "sync" => "Facebook-Games",
        },
    );

    my @targets = (

        # WindowsEditorInstaller contains the Windows Standalone,
        # but keeping this for reference if we want to ship it separately
        # {name=>'Windows-Mono', description=>'Windows Mono Playback Engine', size=>65536},
        { name => 'Android', description => 'Android Playback Engine', size => 65536 },
        { name => 'iOS', description => 'iOS Playback Engine', size => 65536 },
        { name => 'AppleTV', description => 'tvOS Playback Engine', size => 65536 },
        { name => 'Linux', description => 'Linux Playback Engine', size => 312284 },
        { name => 'Mac-Mono', description => 'Mac Playback Engine (Mono Scripting Backend)', size => 65536 },

        { name => 'UWP-.NET', description => 'UWP Playback Engine (.NET Scripting Backend)', size => 65536 },
        { name => 'UWP-IL2CPP', description => 'UWP Playback Engine (IL2CPP Scripting Backend)', size => 65536 },

        { name => 'Vuforia-AR', description => 'Vuforia Augmented Reality Support', size => 65536 },
        { name => 'WebGL', description => 'WebGL Playback Engine', size => 457585 },

        { name => 'Windows-IL2CPP', description => 'Windows Playback Engine (IL2CPP Scripting Backend)', size => 65536 },
        {
            name => 'Facebook-Games',
            description => "Facebook SDK for Gameroom and WebGL\nNote: this will also install the Gameroom application.",
            size => 65536,
            sync => 'WebGL'
        },
    );

    my @targets_enabled = ('Windows');

    foreach my $target (@targets)
    {
        my $tname = $target->{name};
        my $tdescription = $target->{description};
        my $tsize = $target->{size};
        my $path = "$root/build/TargetSupportInstaller/UnitySetup-$tname-Support-for-Editor-$unityVersion.exe";
        if (AmRunningOnBuildServer())
        {
            die("Required file not found: $path\n") unless (-e $path);
        }
        else
        {
            if (!-e $path)
            {
                warn("Required file not found: $path\n");
                next;
            }
        }

        my $install = (grep { $_ eq $tname } @targets_enabled) ? 'true' : 'false';

        my $url = "TargetSupportInstaller/UnitySetup-$tname-Support-for-Editor-$unityVersion.exe";

        push(
            @modules,
            {
                "name" => $tname,
                "build" => $path,
                "description" => $tdescription,
                "url" => $url,
                "mandatory" => "false",
                "install" => $install,
                "cmd" => '"{FILENAME}" /S /D={MODULEDIR}',
                "installedsize" => $tsize,
                "requires_unity" => "true",
            }
        );
    }

    # verify file presence before starting
    my $fail;
    for my $module (@modules)
    {
        if ($module->{build} && !-f $module->{build})
        {
            warn("Expected file not found: $module->{build}\n");
            $fail = 1;
        }
    }
    die("One or more of the component installers are missing.\n") if ($fail && AmRunningOnBuildServer());

    my $inifile = "$root/build/WindowsEditorInstallerSet/$BuildConfig::bootstrapperIniFileWin";
    open my $INI, ">$inifile" or croak "Failed to open $inifile";
    for my $module (@modules)
    {
        my $md5;
        my $size;    # kbytes
        my $size_h;  # human-readable size
        my $installedsize;    # kbytes
        if ($module->{build})
        {
            $size = -s $module->{build};    # in bytes
            $size_h = format_bytes($size);

            if ($ENV{'UNITY_THISISABUILDMACHINE'} && $ENV{'UNITY_THISISABUILDMACHINE'} eq 1)
            {
                $installedsize = CalculateInstalledSizeKB($root, $module->{build});
            }
            else
            {
                warn("Using precalculated component $module->{name} size $module->{installedsize}, which will differ in the final build.\n");
                $installedsize = $module->{installedsize};
            }
            $size = int($size / 1024);      # convert to kbytes

            my $ctx = Digest::MD5->new;
            open my $pkgfh, "<$module->{build}" or croak("Failed to open '$module->{build}' for module '$module->{name}'");
            binmode($pkgfh);
            $ctx->addfile($pkgfh);
            close $pkgfh;

            $md5 = $ctx->hexdigest;
        }
        else
        {
            $size = $module->{size};
            $size_h = $module->{size_h};
            $installedsize = $module->{installedsize};
        }

        $module->{size} = $size;
        $module->{size_h} = $size_h;
        $module->{installedsize} = $installedsize;

        my $title = $BuildConfig::installerPackageTitles{ $module->{name} };
        if (!$title)
        {
            $title = $module->{title};
        }

        print $INI "[$module->{name}]\n";
        print $INI "title=$title\n";

        my @mandatory_fields = qw(description url install mandatory size size_h installedsize cmd);
        my @optional_fields = qw(filename eula eulamessage eulaurl1 eulaurl2 eulaurl3 eulalabel1 eulalabel2 eulalabel3 hidden sync use_vs_editor);

        foreach my $key (@mandatory_fields)
        {
            die("The expected key '$key' is not found in module definition for '$title'\n") unless exists($module->{$key});
            print $INI "$key=$module->{$key}\n";
        }
        if ($md5)
        {
            print $INI "md5=$md5\n";
        }

        foreach my $key (@optional_fields)
        {
            print $INI "$key=$module->{$key}\n" if exists($module->{$key});
        }
    }
    close $INI;
    print("Building WindowsEditorInstallerSet successful\n");
}

sub CalculateInstalledSizeKB
{
    # unzip to temp folder and get total file size
    my ($root, $installerExe) = @_;
    my $tempFolder = "$root/build/InstallerUnzip";

    if (!-e $installerExe)
    {
        croak("Could not find specified installer exe '$installerExe'");
    }

    rmtree($tempFolder) if (-d $tempFolder);
    mkpath($tempFolder);
    local $CWD = $tempFolder;

    (system("$root/External/7z/win32/7za.exe", "x", $installerExe) == 0) or die("Failed to unzip '$installerExe'");
    my $size = 0;
    find(sub { $size += -s if -f $_ }, ".");

    return int($size / 1024);
}

sub CompileVCProj
{
    my $sln = shift(@_);
    my $slnconfig = shift(@_);
    my $incremental = shift(@_);
    my $projectName = shift(@_);
    my @optional = @_;

    # VS100COMNTOOLS is the ONLY way to find where VS is installed.
    my $devenv = ($ENV{"VS100COMNTOOLS"} . "/../IDE/devenv.com");
    if (!-e $devenv)
    {
        croak("Cannot find Visual Studio installation. Please make sure Visual Studio is installed on your machine");
    }

    my $buildCmd = $incremental ? "/build" : "/rebuild";

    print("devenv.exe $sln $buildCmd $slnconfig /project $projectName @optional \n\n");
    system($devenv, $sln, $buildCmd, $slnconfig, '/project', $projectName, @optional) eq 0 or croak("VisualStudio failed to build $projectName project");
}

sub BuildVisualStudioInstallChecker
{
    my ($root) = @_;
    ProgressMessage("Building VisualStudioInstallChecker for Windows Bootstrapper");
    CompileVCProj("$root/Tools/Installers/VisualStudioInstallChecker/VisualStudioInstallChecker.sln", "Release", 0, "VisualStudioInstallChecker");
}

sub BuildWindowsBootstrapper
{
    my ($root, $targetPath, $options, $win64) = @_;
    my $target = "$targetPath/UnityDownloadAssistant.exe";

    ProgressMessage("Creating Windows Bootstrapper at path: $target");
    WriteUnityPathsFile($options, $root);

    BuildVisualStudioInstallChecker($root);

    local $CWD = "$root/Tools/Installers/WindowsBootstrapper";
    system("$root/External/Installers/NSIS/builds/makensis.exe", "/V3", "SetupBootstrapper.nsi") eq 0 or croak("Failed creating bootstrapper");
    if (not -e $target)
    {
        croak("Creating bootstrapper returned succesfully, however the produced installer is not found at $target\n");
    }

    ProgressMessage("Signing final bootstrapper");
    SignFile($target);
}

sub BuildWindowsDocumentationInstaller
{
    my ($root, $targetPath, $options) = @_;

    ProgressMessage("Making DocCombiner");
    my $platform = ($options->{platform} or "win64");
    Jam($root, 'DocCombiner', $options->{codegen}, $platform, $options->{incremental});
    UCopy("$root/build/WindowsDocCombiner/DocCombiner.exe", "$root/build/UserDocumentation/Documentation/");

    WriteUnityPathsFile($options, $root);
    local $CWD = "$root/Tools/Installers/WindowsEditor";

    my $target = "$targetPath/UnityDocumentationSetup.exe";
    ProgressMessage("Creating Documentation installer at path: $target");
    system("$root/External/Installers/NSIS/builds/makensis.exe", "/V3", "SetupDocumentation.nsi") eq 0 or croak("Failing creating Documentation installer");
    if (not -e $target)
    {
        croak("Creating installer returned succesfully, however the produced installer is not found at $target\n");
    }

    ProgressMessage("Signing final installer");
    SignFile($target);
}

sub BuildWindowsMonoDevelopInstaller
{
    my ($root, $targetPath, $options, $win64) = @_;

    if (!-e "$root/build/WindowsMonoDevelop/MonoDevelop" && -e "$root/MonoDevelopBinaries/MonoDevelop.zip")
    {
        ProgressMessage("Unpacking $root/MonoDevelopBinaries/MonoDevelop.zip");
        mkpath("$root/build/WindowsMonoDevelop");
        system("cd $root/build/WindowsMonoDevelop && $root/External/7z/win32/7za.exe x $root/MonoDevelopBinaries/MonoDevelop.zip");
        $? == 0 or die("Unpacking $root/MonoDevelopBinaries/MonoDevelop.app.tar.gz failed\n");
    }

    my $monodevpath = "$root/build/WindowsMonoDevelop/MonoDevelop";
    if (!-e $monodevpath)
    {
        die("MonoDevelop not found at $monodevpath\n");
    }

    WriteUnityPathsFile($options, $root);
    local $CWD = "$root/Tools/Installers/WindowsEditor";

    my $target = "$targetPath/UnityMonoDevelopSetup.exe";
    ProgressMessage("Creating MonoDevelop installer at path: $target");
    system("$root/External/Installers/NSIS/builds/makensis.exe", "/V3", "SetupMonoDevelop.nsi") eq 0 or croak("Failing creating MonoDevelop installer");
    if (not -e $target)
    {
        croak("Creating installer returned succesfully, however the produced installer is not found at $target\n");
    }

    ProgressMessage("Signing final installer");
    SignFile($target);
}

sub BuildWindows64EditorInstaller
{
    my ($root, $targetPath, $options) = @_;
    BuildWindowsEditorInstaller_shared($root, $targetPath, $options, 1);
}

1;
