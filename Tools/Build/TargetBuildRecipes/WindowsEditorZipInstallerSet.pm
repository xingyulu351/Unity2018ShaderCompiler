package WindowsEditorZipInstallerSet;

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
use Tools qw (XBuild UCopy Jam AmRunningOnBuildServer ProgressMessage ReplaceText);
use Tools qw (PackageAndSendResultsToBuildServer BuildBundleFileList);
use Licenses qw(VerifyExternalLicenses BuildLicensesFile ExportExternalLicenses);
use Carp qw(croak carp);
use File::Find;
use Number::Bytes::Human qw(format_bytes);

use BuildConfig;
our $unityVersion = $BuildConfig::unityVersion;

sub Register
{
    Main::RegisterTarget(
        "WindowsEditorZipInstallerSet",
        {
            windowsbuildfunction => \&BuildWindowsEditorZipInstallerSet,
            dependencies => []
        }
    );
}

sub BuildWindowsEditorZip
{
    my ($root, $zipApp, $targetPath, $editorName, $editorFolder) = @_;
    my $zipName = "$editorName.zip";
    my $editorFiles = "$editorFolder/";

    ProgressMessage("Zipping up Windows Editor Zip Installer");

    my @list = (
        "$zipApp", "a", "-r", "-xr!.git",
        "-xr!.hg", "-xr!.hglf", "-xr!*.testlog", "-xr!*.ilk",
        "-xr!*.pdb", "-x!Temp", "-x!etc", "-xr0!*node_modules/*/history",
        "-xr0!*node_modules/*/changelog", "-xr0!*node_modules/*/*.md", "-xr0!*node_modules/*/*.markdown", "-xr0!*node_modules/*/readme.*",
        "-xr0!*node_modules/*/LICENSE.*", "-xr0!*node_modules/*/LICENSE", "-xr0!*node_modules/*/tests", "-xr0!*node_modules/*/*test*.js",
        "-xr0!*node_modules/*/test", "-xr0!*node_modules/*/.npmignore", "-xr0!*node_modules/*/appveyor.yml", "-xr0!*node_modules/*/.travis.yml",
        "$targetPath/$zipName", "$editorFiles"
    );
    system(@list) and croak("Failed to zip up windows editor");

    system("$zipApp", "rn", "$targetPath/$zipName", "WindowsEditor/", "Editor/")
        and croak("Failed to rename WindowsEditor to Editor");

    ProgressMessage("Adding pdbs to Windows Editor Zip Installer");

    # Add back the required pdbs
    if (-e "$editorFolder/Unity_x64_s.pdb")
    {

        #this file is created only if UNITY_THISISABUILDMACHINE=1
        system("$zipApp", "a", "$targetPath/$zipName", "$editorFolder/Unity_x64_s.pdb") and croak("Failed to add Unity_x64_s.pdb");
        system("$zipApp", "rn", "$targetPath/$zipName", "Unity_x64_s.pdb", "unity_x64.pdb")
            and croak("Failed to rename Unity_x64_s.pdb to unity_x64.pdb");
    }
    if (-e "$editorFolder/Unity_x86_s.pdb")
    {

        #this file is created only if UNITY_THISISABUILDMACHINE=1
        system("$zipApp", "a", "$targetPath/$zipName", "$editorFolder/Unity_x86_s.pdb") and croak("Failed to add Unity_x86_s.pdb");
        system("$zipApp", "rn", "$targetPath/$zipName", "Unity_x86_s.pdb", "unity_x86.pdb")
            and croak("Failed to rename Unity_x86_s.pdb to unity_x86.pdb");
    }

    my $monoPdb = "$editorFolder/Data/Mono/EmbedRuntime/mono.pdb";
    if (-e $monoPdb)
    {
        system("$zipApp", "a", "$targetPath/$zipName", $monoPdb) and croak("Failed to zip up mono.pdb");
        system("$zipApp", "rn", "$targetPath/$zipName", "mono.pdb", "Editor/mono.pdb")
            and croak("Failed to rename mono.pdb to Editor/mono.pdb");
    }
    my $mono2Pdb = "$editorFolder/Data/MonoBleedingEdge/EmbedRuntime/mono-2.0-bdwgc.pdb";
    if (-e $mono2Pdb)
    {
        system("$zipApp", "a", "$targetPath/$zipName", $mono2Pdb) and croak("Failed to zip up mono-2.0-bdwgc.pdb");
        system("$zipApp", "rn", "$targetPath/$zipName", "mono-2.0-bdwgc.pdb", "Editor/mono-2.0-bdwgc.pdb")
            and croak("Failed to rename mono-2.0-bdwgc.pdb to Editor/mono-2.0-bdwgc.pdb");
    }

    my $monoEtc = "$editorFolder/Data/Mono/etc";
    if (-e $monoEtc)
    {
        system("$zipApp", "a", "$targetPath/$zipName", $monoEtc) and croak("Failed to zip up Mono/etc");
        system("$zipApp", "rn", "$targetPath/$zipName", "etc", "Editor/Data/Mono/etc")
            and croak("Failed to rename etc to Editor/Data/Mono/etc");
    }

    my $monoBleedingEtc = "$editorFolder/Data/MonoBleedingEdge/etc";
    if (-e $monoEtc)
    {
        system("$zipApp", "a", "$targetPath/$zipName", $monoBleedingEtc) and croak("Failed to zip up MonoBleedingEdge/etc");
        system("$zipApp", "rn", "$targetPath/$zipName", "etc", "Editor/Data/MonoBleedingEdge/etc")
            and croak("Failed to rename etc to Editor/Data/MonoBleedingEdge/etc");
    }

    # Add the vcredist and dotnet files
    ProgressMessage("Adding vcredists and .net installation files to Windows Editor Zip Installer");

    # Those files are used during the installation only and should be removed after a successful install
    my @files = (
        "$root/External/Microsoft/vcredist2010/builds/vcredist_x64.exe", "$root/External/Microsoft/vcredist2010/builds/vcredist_x86.exe",
        "$root/External/Microsoft/vcredist2013/builds/vcredist_x64.exe", "$root/External/Microsoft/vcredist2013/builds/vcredist_x86.exe",
        "$root/External/Microsoft/vcredist2015/builds/vc_redist.x64.exe", "$root/External/Microsoft/vcredist2015/builds/vc_redist.x86.exe",
        "$root/External/Microsoft/dotnet45/builds/dotNetFx45_Full_setup.exe"
    );
    foreach (@files)
    {
        my $filename = basename($_);
        my $folders = join('/', (split /\//, $_)[-4 .. -2]);

        ProgressMessage("Adding $_ to ExternalInstallers/$filename");
        system("$zipApp", "a", "$targetPath/$zipName", $_) and croak("Failed to zip up $_");
        system("$zipApp", "rn", "$targetPath/$zipName", $filename, "ExternalInstallers/$folders/$filename")
            and croak("Failed to rename $filename to ExternalInstallers/$folders/$filename");
    }

    # Add the default playback engine = Windows Standalone Support
    ProgressMessage("Adding Windows Standalone Support to Windows Editor Zip Installer");
    system("$zipApp", "a", "$targetPath/$zipName", "$root/build/WindowsStandaloneSupport/") and croak("Failed to add Windows Standalone Support");
    system("$zipApp", "rn", "$targetPath/$zipName", "WindowsStandaloneSupport/", "Editor/Data/PlaybackEngines/WindowsStandaloneSupport/")
        and croak("Failed to move Windows Standalone Support to the playback engines folder");
    cleanWindowsStandaloneSupport($root, $zipApp, $targetPath, "WindowsStandaloneSupport/", $zipName);

}

sub cleanPdbs
{
    my ($root, $zipApp, $componentFolder, $folder, $zipPath) = @_;
    if (-e $folder)
    {
        ProgressMessage("folder exists $componentFolder/$folder");

        system("$zipApp", "a", "$zipPath", "$componentFolder/$folder/", "-r")
            and croak("Failed to add the  files from $componentFolder/$folder in $zipPath");

        foreach my $file (glob("$componentFolder/$folder/*.pdb"))
        {
            my $fileName = substr($file, rindex($file, "/") + 1);

            if (index($file, "_s") == -1 || index($file, "_d") == -1 || index($file, "_d_s") == -1)
            {
                system("$zipApp", "d", "$zipPath", "$folder/$fileName") and croak("Failed to delete file $file from $zipPath");
            }
            else
            {
                my $newFileName = join("", split("_s", $fileName));
                system("$zipApp", "rn", "$zipPath", "$folder/$fileName", "$folder/$newFileName")
                    and croak("Failed to rename the file $file to $newFileName");
            }
        }
    }
}

sub cleanWindowsStandaloneSupport
{
    my ($root, $zipApp, $targetPath, $componentFolder, $zipName) = @_;

    system("$zipApp", "d", "$targetPath/$zipName", "Source/WindowsPlayer/*.VC.db")
        and croak("Failed to delete file Source/WindowsPlayer/*.VC.db from $targetPath/$zipName");

    # add back the stripped pdbs and rename without _s
    my @pdbFiles = (
        "Variations/win32_development_mono/", "Variations/win32_nondevelopment_mono/",
        "Variations/win64_development_mono/", "Variations/win64_nondevelopment_mono/",
        "Variations/win32_development_il2cpp/", "Variations/win32_nondevelopment_il2cpp/",
        "Variations/win64_development_il2cpp/", "Variations/win64_nondevelopment_il2cpp/"
    );
    foreach my $file (@pdbFiles)
    {
        cleanPdbs($root, $zipApp, $componentFolder, $file, "$targetPath/$zipName");
    }
}

sub BuildWindowsComponentZip
{
    my ($root, $zipApp, $targetPath, $componentName, $componentFolder) = @_;
    my $zipName = "$componentName.zip";
    ProgressMessage("Zipping up $componentName");

    if (-d $componentFolder)
    {

        # verify if there is a ZippedForTeamCity.zip file already
        if (-e "$componentFolder/ZippedForTeamCity.zip")
        {

            # make sure to remove all the _s.pdb and .ilk files
            copy("$componentFolder/ZippedForTeamCity.zip", "$targetPath/$zipName") or die "Copy of the zip failed: $!";
            system("$zipApp", "d", "$targetPath/$zipName", ".ilk", "_s.pdb", "-r")
                and croak("Failed to clean up component zip from $componentFolder");
        }
        else
        {
            system("$zipApp", "a", "-r", "-xr!.git", "-xr!.hg", "-xr!.hglf", "-xr!*.ilk", "-xr!*_d.pdb", "-xr!*_d_s.pdb", "-xr!*_s.pdb",
                "$targetPath/$zipName", "$componentFolder/*")
                and croak("Failed to zip up component from $componentFolder");
        }

        # component exceptions
        if ($componentName eq "MetroBuildSupport")
        {

            # add back the stripped pdbs and rename without _s
            my @pdbFiles = (
                "Players/Windows81/Windows81/ARM/debug/dotnet", "Players/Windows81/Windows81/ARM/release/dotnet",
                "Players/Windows81/Windows81/ARM/master/dotnet", "Players/Windows81/Windows81/x86/debug/dotnet",
                "Players/Windows81/Windows81/x86/release/dotnet", "Players/Windows81/Windows81/x86/master/dotnet",
                "Players/WindowsPhone81/WP81/ARM/debug/dotnet", "Players/WindowsPhone81/WP81/ARM/release/dotnet",
                "Players/WindowsPhone81/WP81/ARM/master/dotnet", "Players/WindowsPhone81/WP81/x86/debug/dotnet",
                "Players/WindowsPhone81/WP81/x86/release/dotnet", "Players/WindowsPhone81/WP81/x86/master/dotnet",
                "Players/UAP/dotnet/UAP/ARM/debug/dotnet", "Players/UAP/dotnet/UAP/ARM/release/dotnet",
                "Players/UAP/dotnet/UAP/ARM/master/dotnet", "Players/UAP/dotnet/UAP/x86/debug/dotnet",
                "Players/UAP/dotnet/UAP/x86/release/dotnet", "Players/UAP/dotnet/UAP/x86/master/dotnet",
                "Players/UAP/dotnet/UAP/x64/debug/dotnet", "Players/UAP/dotnet/UAP/x64/release/dotnet",
                "Players/UAP/dotnet/UAP/x64/master/dotnet", "Players/UAP/il2cpp/UAP/ARM/debug/il2cpp",
                "Players/UAP/il2cpp/UAP/ARM/release/il2cpp", "Players/UAP/il2cpp/UAP/ARM/master/il2cpp",
                "Players/UAP/il2cpp/UAP/x86/debug/il2cpp", "Players/UAP/il2cpp/UAP/x86/release/il2cpp",
                "Players/UAP/il2cpp/UAP/x86/master/il2cpp", "Players/UAP/il2cpp/UAP/x64/debug/il2cpp",
                "Players/UAP/il2cpp/UAP/x64/release/il2cpp", "Players/UAP/il2cpp/UAP/x64/master/il2cpp"
            );
            foreach my $file (@pdbFiles)
            {
                cleanPdbs($root, $zipApp, $componentFolder, $file, "$targetPath/$zipName");
            }
        }
        elsif ($componentName eq "WindowsStandaloneBuildSupport")
        {
            cleanWindowsStandaloneSupport($root, $zipApp, $targetPath, $componentFolder, $zipName);
        }
        elsif ($componentName eq "WindowsWebGLBuildSupport")
        {
            my @filesToDelete =
                ("BuildTools/Emscripten_FastComp_Mac/", "BuildTools/Emscripten_FastComp_Linux/", "BuildTools/Emscripten_Mac/", "BuildTools/Emscripten_Linux/");
            foreach my $file (@filesToDelete)
            {
                system("$zipApp", "d", "$targetPath/$zipName", "$file")
                    and croak("Failed to delete folder $file from $targetPath/$zipName");
            }
        }
    }
}

#The buildinfo file has this inside:
#URL: http://hg-mirror-agents-dub.bf.unity3d.com/all-unity
#SVN: b26894a06a5e
#Version: 2019.2.0a6
#Branch: trunk
sub WriteBuildInfoFile
{
    my ($root, $targetPath) = @_;

    ProgressMessage("Creating the buildinfo file");
    my $buildinfofile = "$targetPath/buildInfo";
    open my $BI, ">$buildinfofile" or croak "Failed to open $buildinfofile";
    print $BI "URL: \n";
    print $BI "SVN: \n";
    print $BI "Version: $unityVersion\n";
    print $BI "Branch: \n";
    close $BI;
    print("Building WindowsEditorZipInstallerSet build info file successful\n");
}

# The ini file states what components are available for a given release
sub WriteIniFile
{
    my ($root, $targetPath, $options, %components) = @_;

    ProgressMessage("Creating the ini file");

    # Generate the bootstrapper ini file
    my @modules = (
        {
            "name" => "Unity",
            "build" => "WindowsEditorZipInstaller.zip",
            "rawfiles" => "$root/build/WindowsEditor",
            "description" => "Unity Editor for building your games",
            "url" => "WindowsEditorZipInstaller.zip",
            "mandatory" => "false",
            "install" => "true",
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
        # {name=>'Windows-Mono', description=>'Windows Mono Playback Engine', size=>65536, build => ''},
        {
            name => 'Android',
            description => 'Android Playback Engine',
            size => 65536,
            componentName => "AndroidBuildSupport"
        },
        {
            name => 'iOS',
            description => 'iOS Playback Engine',
            size => 65536,
            componentName => "iOSBuildSupport"
        },
        {
            name => 'AppleTV',
            description => 'tvOS Playback Engine',
            size => 65536,
            componentName => "TvOSBuildSupport"
        },
        {
            name => 'Linux',
            description => 'Linux Playback Engine',
            size => 312284,
            componentName => "LinuxStandaloneBuildSupport"
        },
        {
            name => 'Mac-Mono',
            description => 'Mac Playback Engine (Mono Scripting Backend)',
            size => 65536,
            componentName => "MacStandaloneBuildSupport"
        },
        {
            name => 'UWP-.NET',
            description => 'UWP Playback Engine (.NET Scripting Backend)',
            size => 65536,
            componentName => "MetroBuildSupport"
        },
        {
            name => 'UWP-IL2CPP',
            description => 'UWP Playback Engine (IL2CPP Scripting Backend)',
            size => 65536,
            componentName => "MetroBuildSupport"
        },
        {
            name => 'Vuforia-AR',
            description => 'Vuforia Augmented Reality Support',
            size => 65536,
            componentName => "VuforiaBuildSupport"
        },
        {
            name => 'WebGL',
            description => 'WebGL Playback Engine',
            size => 457585,
            componentName => "WebGLBuildSupport"
        },
        {
            name => 'Windows-IL2CPP',
            description => 'Windows Playback Engine (IL2CPP Scripting Backend)',
            size => 65536,
            componentName => "MetroBuildSupport"
        },
        {
            name => 'Facebook-Games',
            description => "Facebook SDK for Gameroom and WebGL\nNote: this will also install the Gameroom application.",
            size => 65536,
            sync => 'WebGL',
            componentName => "FacebookBuildSupport"
        },
    );

    my @targets_enabled = ('Windows');

    foreach my $target (@targets)
    {
        my $tname = $target->{name};
        my $tdescription = $target->{description};
        my $tsize = $target->{size};
        my $path = "$target->{componentName}\.zip";
        if (AmRunningOnBuildServer())
        {
            die("Required file not found: $targetPath/$path\n") unless (-e "$targetPath/$path");
        }
        else
        {
            if (!-e "$targetPath/$path")
            {
                warn("Required file not found: $targetPath/$path\n");
                next;
            }
        }

        my $install = (grep { $_ eq $tname } @targets_enabled) ? 'true' : 'false';

        my $url = "$target->{componentName}\.zip";

        push(
            @modules,
            {
                "name" => $tname,
                "build" => $path,
                "description" => $tdescription,
                "url" => $url,
                "mandatory" => "false",
                "install" => $install,
                "installedsize" => $tsize,
                "requires_unity" => "true",
                "rawfiles" => $components{ $target->{componentName} }
            }
        );
    }

    my $modulesSize = @modules;

    # verify file presence before starting
    my $fail;
    for my $module (@modules)
    {
        if ($module->{build} && !-e "$targetPath/$module->{build}")
        {
            warn("Expected file not found: $module->{build}\n");
            $fail = 1;
        }
    }
    die("One or more of the component zip files are missing.\n") if ($fail && AmRunningOnBuildServer());

    my $inifile = "$targetPath/$BuildConfig::bootstrapperIniFileWin";
    open my $INI, ">$inifile" or croak "Failed to open $inifile";
    for my $module (@modules)
    {
        ProgressMessage("processing $module->{name}");

        my $size;    # kbytes
        my $size_h;  # human-readable size
        my $installedsize = 0;    # kbytes
        if ($module->{build})
        {
            if (-e "$targetPath/$module->{build}")
            {
                $size = -s "$targetPath/$module->{build}";    # in bytes
            }
            else
            {
                $size = 0;
            }
            $size_h = format_bytes($size);
            $size = int($size / 1024);                        # convert to kbytes

            my $ctx = Digest::MD5->new;
            open my $pkgfh, "<$targetPath/$module->{build}" or croak("Failed to open '$module->{build}' for module '$module->{name}'");
            binmode($pkgfh);
            $ctx->addfile($pkgfh);
            close $pkgfh;
            $module->{md5} = $ctx->hexdigest;
        }
        else
        {
            $size = $module->{size};
            $size_h = $module->{size_h};
        }

        if ($module->{rawfiles} && -d $module->{rawfiles})
        {
            $installedsize = int(dirSize($module->{rawfiles}) / 1024);    # convert to kbytes
        }
        elsif ($module->{installedsize})
        {
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

        my @mandatory_fields = qw(description url install mandatory size size_h installedsize);
        my @optional_fields = qw(md5 cmd filename eula eulamessage eulaurl1 eulaurl2 eulaurl3 eulalabel1 eulalabel2 eulalabel3 hidden sync use_vs_editor);

        foreach my $key (@mandatory_fields)
        {
            die("The expected key '$key' is not found in module definition for '$title'\n") unless exists($module->{$key});
            print $INI "$key=$module->{$key}\n";
        }

        foreach my $key (@optional_fields)
        {
            print $INI "$key=$module->{$key}\n" if exists($module->{$key});
        }
    }
    close $INI;
    print("Building WindowsEditorZipInstallerSet ini file successful\n");
}

sub dirSize
{
    my ($dir) = @_;
    my ($size) = 0;
    my ($fd);

    opendir($fd, $dir) or die "$!";

    for my $item (readdir($fd))
    {
        next if ($item =~ /^\.\.?$/);

        my ($path) = "$dir/$item";

        $size += ((-d $path) ? dirSize($path) : (-f $path ? (stat($path))[7] : 0));
    }

    closedir($fd);

    return ($size);
}

sub BuildWindowsEditorZipInstallerSet
{
    my ($root, $targetPath, $options) = @_;
    ProgressMessage("Building Windows Editor Zip Installer Set");

    # make sure to have a subfolder
    $targetPath = "$targetPath/artifacts";

    my $zipApp = "$root/External/7z/win64/7za.exe";

    # At this point, this script expects to have all dependencies,
    # at the following locations:
    #
    # Unity Editor: build/WindowsEditor/
    # User Documentation: build/UserDocumentation
    # Standard Assets: build/StandardAssets
    # Example Project: build/ExampleProject
    # Mono Develop: targetPath/MonoDevelop.zip !!
    # Android Build Support: build/AndroidPlayer
    # iOS Build Support: build/iOSSupport
    # TvOS Build Support: build/AppleTVSupport
    # Winbdows Build Support: build/WindowsStandaloneSupport
    # Linux Build Support: build/LinuxStandaloneSupport
    # Mac Build Support: build/MacStandaloneSupport
    # Metro Build Support: build/MetroSupport
    # Tizen Build Support: build/TizenPlayer
    # Vuforia Build Support: build/Vuforia
    # WebGL Build Support: build/WebGLSupport
    # Facebook Build Support: build/Facebook

    #create a zip archive of the editor
    BuildWindowsEditorZip($root, $zipApp, $targetPath, "WindowsEditorZipInstaller", "$root/build/WindowsEditor");

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
        MetroBuildSupport => "$root/build/MetroSupport",
        TizenBuildSupport => "$root/build/TizenPlayer",
        VuforiaBuildSupport => "$root/build/VuforiaSupport",
        WebGLBuildSupport => "$root/build/WebGLSupport",
        FacebookBuildSupport => "$root/build/Facebook",
    );

    for my $component (keys %components)
    {
        BuildWindowsComponentZip($root, $zipApp, $targetPath, $component, $components{$component});
    }

    # add the ini file
    WriteIniFile($root, $targetPath, $options, %components);

    # add the buildinfo file
    WriteBuildInfoFile($root, $targetPath);
}
1;
