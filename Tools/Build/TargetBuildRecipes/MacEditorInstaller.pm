package MacEditorInstaller;

use strict;
use warnings;
use File::Path qw(mkpath rmtree);
use File::Basename qw(dirname basename);
use File::Spec;
use File::chdir;
use Carp qw (croak);
use lib File::Spec->rel2abs(dirname($0) . "/../../..") . '/External/Perl/lib';
use OTEE::BuildUtils qw(build_i386 build_x86_64 build_x86_64_only svnGetRevision);
use lib File::Spec->rel2abs(dirname($0) . "/..");
use MacTools
    qw (PostbuildCleanup BuildPackage BuildPackageNoRelocate BuildMacProductInstaller CodeSignBundle NotarizeApp SignPackage Cleanup StripEditor BuildDmg SetCustomFileIcon BuildAndPatchComponentPlist);
use Tools qw (SetupPlaybackEngines UCopy UMove AmRunningOnBuildServer ProgressMessage ReplaceText ProgressMessage);
use Digest::MD5;
use Time::localtime;
use UserDocumentation;
use TargetSupportInstaller;
use MacInstallerLicensePlugin qw(BuildMacInstallerLicensePlugin);

# this comes from xcodeproj
my $bootstrapperBundleName = 'Unity Download Assistant';

my $assemblyPath = "build/temp";
my $unityAssemblyPath = "$assemblyPath/Files/Unity";
my $unityAppPath = "build/MacEditor/Unity.app/Contents/MacOS/Unity";

use BuildConfig;
our $unityVersion = $BuildConfig::unityVersion;
our $copyright = $BuildConfig::copyright;

sub Register
{
    Main::RegisterTarget(
        "MacEditorInstallerSet",
        {
            macbuildfunction => \&BuildMacEditorInstallerSet,
            dependencies => ["macbootstrapper", "maceditorinstaller", "macdocumentationinstaller",],
        }
    );
    Main::RegisterTarget(
        "MacEditorInstaller",
        {
            macbuildfunction => \&BuildMacEditorInstaller,
            dependencies => ["maceditor", "MacStandaloneSupport",],
            options => {
                "codegen" => ["debug", "release"],
                "buildSudoPass" => [0, 1],
                "incremental" => [0, 1],
                "playbackengines" => ["MacStandaloneSupport"]
            }
        }
    );
    Main::RegisterTarget(
        "MacEditorTargetInstaller",
        {
            macbuildfunction => \&TargetSupportInstaller::BuildTargetSupportInstallerMac,
            options => { "playbackengines" => ["WebGLSupport"] }
        }
    );

    Main::RegisterTarget(
        "MacBootstrapper",
        {
            macbuildfunction => \&BuildMacBootstrapper,
            options => {}
        }
    );
}

sub BuildMacEditorCommon
{
    my ($sourcePackage, $installerName, $root, $options) = @_;
    ProgressMessage("Building Mac Editor - $sourcePackage - $installerName");

    rmtree("$assemblyPath/disk_image_source_$installerName");
    mkpath("$unityAssemblyPath");

    # Cleanup the bundle location
    my $cmd = "rm -rf '$unityAssemblyPath/Unity.app'";
    print("Running command: $cmd\n");
    system($cmd) and die("rm failed: $cmd\n");

    my $pluginsPath = BuildMacInstallerLicensePlugin($root, $assemblyPath);

    my $playbackEnginePath = "$unityAssemblyPath/Unity.app/Contents/PlaybackEngines";
    rmtree("$playbackEnginePath");
    mkpath("$playbackEnginePath");

    if (!AmRunningOnBuildServer())
    {

        # hack: delete the broken symlink
        unlink("$root/build/MacEditor/Unity.app/Contents/Documentation");
    }

    # UCopy doesn't preserve symlinks, which we need here, so copy manually.
    $cmd = "cp -af '$root/build/MacEditor/Unity.app' '$unityAssemblyPath'";
    print("Running command: $cmd\n");
    system($cmd) and die("cp failed: $cmd\n");

    # For TC. It has problems preserving execute permissions when unzipping
    system("chmod", "+x", "$unityAssemblyPath/Unity.app/Contents/MacOS/Unity");
    system("chmod", "+x", "$unityAssemblyPath/Unity.app/Contents/Tools/*");
    system("chmod", "+x", "$unityAssemblyPath/Unity.app/Contents/Tools/Beast/thebeast");
    system("chmod", "+x", "$unityAssemblyPath/Unity.app/Contents/PlaybackEngines/iOSSupport/Tools/OSX/mono-xcompiler");

    UserDocumentation::BuildUserDocumentation_MonoDoc($unityAssemblyPath);

    ## Process dependencies
    SetupPlaybackEngines($root, $options, $playbackEnginePath);

    # Setup symlink to Unity Bug Reporter
    if (-e "$unityAssemblyPath/Unity.app/Contents/BugReporter/Unity Bug Reporter.app")
    {
        local $CWD = $unityAssemblyPath;
        system('ln -s Unity.app/Contents/BugReporter/Unity\ Bug\ Reporter.app Unity\ Bug\ Reporter.app');
    }
    else
    {
        die "$unityAssemblyPath/Unity.app/Contents/BugReporter/Unity Bug Reporter.app not found\n";
    }

    {
        ProgressMessage("Adding Npm Symlink");
        local $CWD = "$unityAssemblyPath/Unity.app/Contents/Tools/nodejs/bin";
        unlink("npm") if (-e "npm" || -l "npm");
        symlink("../lib/node_modules/npm/bin/npm-cli.js", "npm") or croak("Failed to link Npm");
    }

    {
        ProgressMessage("Adding UserDocumentation Symlink");
        my $symlinkname = "$unityAssemblyPath/Unity.app/Contents/Documentation";
        rmtree($symlinkname);

        unlink($symlinkname) if (-l $symlinkname);
        symlink("../../Documentation", $symlinkname) or croak("Linking Documentation");
    }

    ProgressMessage("Adding Plugin Interface Header Files");
    mkdir("$unityAssemblyPath/Unity.app/Contents/PluginAPI");
    $cmd = "cp -Rfp $root/Runtime/PluginInterface/Headers/ $unityAssemblyPath/Unity.app/Contents/PluginAPI/";
    print "$cmd\n";
    system($cmd) and die("Failed to copy plugin PluginAPI\n");

    my $unityAppFrameworks = "$root/$unityAssemblyPath/Unity.app/Contents/Frameworks";

    # Remove the engine and editor mdb file. This will make the stacktraces nicer because we don't print line / file info for engine stack traces.
    ProgressMessage("pwd: " . `pwd`);
    ProgressMessage("unlinking: " . "$unityAppFrameworks/Managed/UnityEngine.dll.mdb");

    my $mdb = "$unityAppFrameworks/Managed/UnityEngine.dll.mdb";
    if (-e $mdb)
    {
        unlink($mdb) or croak("Failed to remove engine/editor mdb: $mdb");
    }

    # Remove sym files
    rmtree("$unityAssemblyPath/Unity.app/Contents/MacOS/Unity.xSYM");

    ProgressMessage("Stripping libraries ... ");

    StripEditor(File::Spec->rel2abs("$unityAppFrameworks/../Mono/bin/mono"));
    StripEditor("$unityAppFrameworks/Mono/MonoEmbedRuntime/osx/libmono.0.dylib");

    UCopy("$root/Tools/Licenses/License.rtf", "$assemblyPath/InstallerSources/Unity/Resources/License.rtf");
    UCopy("$root/Tools/Installers/InstallerAssets/background.tiff", "$assemblyPath/InstallerSources/Unity/Resources/");

    # Fix ownerships
    system("chmod -R g+w $assemblyPath/Files") eq 0 or croak("Could not change file permissions.\n");

    chdir("$assemblyPath");

    my $bundlePath = "$root/$unityAssemblyPath/Unity.app";

    CodeSignBundle($bundlePath, 0);

    my $comp_plist = "$assemblyPath/InstallerSources/Unity/component.plist";
    BuildAndPatchComponentPlist("$assemblyPath/Files", $comp_plist);

    BuildPackage("$assemblyPath/Unity.pkg.tmp", "$assemblyPath/Files", "/Applications", "$assemblyPath/InstallerSources/Unity/Scripts",
        "com.unity3d.UnityEditor5.x", $comp_plist);

    # signing component package is not required, as we sign the product package
    #SignPackage ("$assemblyPath/Unity.pkg.tmp");
    my @packages = ("$assemblyPath/Unity.pkg.tmp");
    BuildMacProductInstaller(
        targetPath => "$assemblyPath/Unity.pkg",
        sourceFiles => "$assemblyPath/InstallerSources/Unity",
        packageList => \@packages,
        pluginsPath => $pluginsPath
    );
    SetCustomFileIcon("$assemblyPath/Unity.pkg", "$root/Tools/Installers/InstallerAssets/installer.icns");

    chdir($root);
}

sub BuildMacEditorInstallerSet
{
    my ($root, $targetPath, $options) = @_;
    $assemblyPath = "$root/$assemblyPath";

    my %svnInfo = svnGetRevision();

    # At this point, this script expects to have all dependencies as pkg files,
    # at the following locations:
    #
    # Unity: build/MacEditorInstaller/Unity.pkg

    # Generate the bootstrapper ini file
    my @modules = (
        {
            "name" => "Unity",
            "build" => "$root/build/MacEditorInstaller/Unity.pkg",
            "description" => "Unity Editor",
            "url" => "MacEditorInstaller/Unity.pkg",
            "mandatory" => "false",
            "install" => "true",
            "version" => $unityVersion,
        },
        {
            "name" => "Mono",
            "title" => "Mono for Visual Studio for Mac",
            "description" => "Required for Visual Studio for Mac",
            "url" => "https://go.microsoft.com/fwlink/?linkid=857641",
            "extension" => "pkg",
            "mandatory" => "false",
            "install" => "false",
            "size" => "500000000",
            "installedsize" => "1524000000",
            "hidden" => "true",
            "sync" => "VisualStudio",
        },
        {
            "name" => "VisualStudio",
            "title" => "Visual Studio for Mac",
            "description" => "Script IDE with Unity integration and debugging support. Also installs Mono, required for Visual Studio for Mac to run",
            "url" => "https://go.microsoft.com/fwlink/?linkid=873167",
            "extension" => "dmg",
            "eulamessage" => "Please review and accept the license terms before downloading and installing Visual Studio for Mac and Mono.",
            "eulalabel1" => "Visual Studio for Mac License Terms",
            "eulaurl1" => "https://www.visualstudio.com/license-terms/visual-studio-mac-eula/",
            "eulalabel2" => "Mono License Terms",
            "eulaurl2" => "http://www.mono-project.com/docs/faq/licensing/",
            "mandatory" => "false",
            "install" => "true",
            "size" => "820000000",    # Includes VS and Mono size
            "installedsize" => "2304000000",    # Includes VS and Mono installedsize
            "appidentifier" => "com.microsoft.visual-studio",
        },
    );

    if (AmRunningOnBuildServer())
    {
        my @targets = (

            # MacEditorInstaller contains the Mac Standalone,
            # but keeping this for reference if we want to ship it separately
            # {name=>'Mac-Mono'},
            { name => 'Android' },
            { name => 'iOS' },
            { name => 'AppleTV' },
            { name => 'Linux' },
            { name => 'Mac-IL2CPP' },

            { name => 'Vuforia-AR' },
            { name => 'WebGL' },
            { name => 'Windows-Mono' },
            { name => 'Facebook-Games' },
        );

        my @targets_enabled = ('Mac');

        foreach my $target (@targets)
        {
            my $tname = $target->{name};
            my $path = "$root/build/MacEditorTargetInstaller/UnitySetup-$tname-Support-for-Editor-$unityVersion.pkg";
            die("Required file not found: $path\n") unless (-e $path);

            my $install = (grep { $_ eq $tname } @targets_enabled) ? 'true' : 'false';

            my $module = {
                "name" => $tname,
                "build" => $path,
                "description" => "Allows building your Unity projects for the $tname platform",
                "url" => "MacEditorTargetInstaller/UnitySetup-$tname-Support-for-Editor-$unityVersion.pkg",
                "mandatory" => "false",
                "install" => $install,
                "requires_unity" => "true",
            };

            if ($tname eq 'Facebook-Games')
            {
                $module->{optsync_windows} = 'Windows';
                $module->{optsync_webgl} = 'WebGL';
            }
            push(@modules, $module);
        }
    }

    my $inifile = "$root/build/MacEditorInstallerSet/$BuildConfig::bootstrapperIniFileOSX";
    open my $INI, ">$inifile" or croak "Failed to open $inifile";
    for my $module (@modules)
    {
        my $md5 = "";
        my $size;
        my $installedsize;

        if ($module->{build})
        {
            my $ctx = Digest::MD5->new;
            open my $pkgfh, "<$module->{build}" or croak("Failed to open '$module->{build}' for module '$module->{name}'");
            $ctx->addfile($pkgfh);
            close $pkgfh;

            $md5 = "md5=" . $ctx->hexdigest . "\n";
            $size = -s $module->{build};

            my $pkginfo = `installer -verbose -pkginfo -pkg $module->{build}`;
            croak "installer -pkginfo failed on file: $module->{build}" if $?;

            $pkginfo =~ /Size\s+:\s+([0-9]+)\s+KB/ or croak "Failed to get installed size of $module->{build}";
            $installedsize = int($1) * 1000;
        }
        else
        {
            if (!exists $module->{size})
            {
                die("External module $module->{name} doesn't have size field\n");
            }
            if (!exists $module->{installedsize})
            {
                die("External module $module->{name} doesn't have installedsize field\n");
            }
            $size = $module->{size};
            $installedsize = $module->{installedsize};
        }

        my $title = $BuildConfig::installerPackageTitles{ $module->{name} };
        if (!$title)
        {
            $title = $module->{title};
        }
        if (!$title)
        {
            warn("BuildConfig::installerPackageTitles doesn't have key '$module->{name}'\n");
        }
        my $version = "";
        if (exists $module->{version})
        {
            $version = "version=$module->{version}\n";
        }
        my $requires_unity = "";
        if (exists $module->{requires_unity})
        {
            $requires_unity = "requires_unity=$module->{requires_unity}\n";
        }
        my $sync = '';
        if (exists $module->{sync})
        {
            $sync = "sync=$module->{sync}\n";
        }
        my $hidden = '';
        if (exists $module->{hidden})
        {
            $hidden = "hidden=$module->{hidden}\n";
        }
        my $eulamessage = '';
        if (exists $module->{eulamessage})
        {
            $eulamessage = "eulamessage=$module->{eulamessage}\n";
        }
        my $eulalabel1 = '';
        if (exists $module->{eulalabel1})
        {
            $eulalabel1 = "eulalabel1=$module->{eulalabel1}\n";
        }
        my $eulaurl1 = '';
        if (exists $module->{eulaurl1})
        {
            $eulaurl1 = "eulaurl1=$module->{eulaurl1}\n";
        }
        my $eulalabel2 = '';
        if (exists $module->{eulalabel2})
        {
            $eulalabel2 = "eulalabel2=$module->{eulalabel2}\n";
        }
        my $eulaurl2 = '';
        if (exists $module->{eulaurl2})
        {
            $eulaurl2 = "eulaurl2=$module->{eulaurl2}\n";
        }
        my $appidentifier = '';
        if (exists $module->{appidentifier})
        {
            $appidentifier = "appidentifier=$module->{appidentifier}\n";
        }
        my $extension = '';
        if (exists $module->{extension})
        {
            $extension = "extension=$module->{extension}\n";
        }

        my $ini_sec =
            "[$module->{name}]\ntitle=$title\ndescription=$module->{description}\nurl=$module->{url}\ninstall=$module->{install}\nmandatory=$module->{mandatory}\nsize=$size\ninstalledsize=$installedsize\n$version$requires_unity$sync$md5$hidden$eulamessage$eulalabel1$eulaurl1$eulalabel2$eulaurl2$appidentifier$extension";
        map { $ini_sec .= "$_=$module->{$_}\n" } grep /^optsync_/, keys %{$module};
        print $INI $ini_sec;
    }
    close $INI;

    # Copy download.html and substitute variables
    open(my $download_html_in, "<$root/Tools/Installers/download.html") or die $!;
    open(my $download_html_out, ">$root/build/MacEditorInstallerSet/download.html") or die $!;

    my $tm = localtime;
    my $tm_text = sprintf("%04d-%02d-%02d", $tm->year + 1900, ($tm->mon) + 1, $tm->mday);
    while (<$download_html_in>)
    {
        $_ =~ s/__VERSION__/$unityVersion/g;
        $_ =~ s/__REVISION__/$svnInfo{Revision}/g;
        $_ =~ s/__DATE__/$tm_text/g;
        print $download_html_out $_;
    }

    close $download_html_in;
    close $download_html_out;
}

sub BuildMacEditorInstaller
{
    my ($root, $targetPath, $options) = @_;

    $assemblyPath = "$root/$assemblyPath";

    rmtree("$root/build/MacEditorInstaller");
    mkpath("$root/build/MacEditorInstaller");

    # Set up installer config and package location
    rmtree("$assemblyPath/InstallerSources/Unity");
    mkpath("$assemblyPath/InstallerSources");
    UCopy("$root/Tools/Installers/Unity", "$assemblyPath/InstallerSources/Unity");

    BuildMacEditorCommon("Unity.pkg", "unity", $root, $options);
    ProgressMessage("Moving $assemblyPath/Unity.pkg to $targetPath");
    UMove("$assemblyPath/Unity.pkg", "$targetPath");
    ProgressMessage("Move was successful");
}

sub BuildMacBootstrapper
{
    my ($root, $targetPath, $options) = @_;
    my %svnInfo = svnGetRevision();
    my $svnRevision = "$svnInfo{Revision}";

    if (!$svnRevision)
    {
        die("svnGetRevision returned 0 hg revision, which is not an acceptable value\n");
    }

    chdir($root);
    rmtree($targetPath);
    mkpath($targetPath);

    $unityAppPath = "$root/$unityAppPath";

    build_x86_64_only('Tools/Installers/MacBootstrapper/UnityInstaller.xcodeproj', 'UnityInstaller');

    open my $INI, ">Tools/Installers/MacBootstrapper/build/Release/$bootstrapperBundleName.app/Contents/Resources/settings.ini"
        or die "Failed to open Tools/Installers/MacBootstrapper/$bootstrapperBundleName.app/Contents/Resources/settings.ini";

    print $INI "[settings]\n";
    my $mirroridx = 1;
    foreach my $iniloc (@BuildConfig::bootstrapperIniLocations)
    {
        print $INI "ini_url$mirroridx=$iniloc/$svnRevision/$BuildConfig::bootstrapperIniFileOSX\n";
        $mirroridx++;
    }

    close $INI;

    # create dmg
    my $installerName = 'UnityDownloadAssistant';
    UCopy("$root/Tools/Installers/MacBootstrapper/Template.sparseBundle", "$assemblyPath/MacBootstrapper/");
    rmtree("$assemblyPath/disk_image_source_$installerName");
    mkpath("$assemblyPath/disk_image_source_$installerName");
    UCopy("$root/Tools/Installers/MacBootstrapper/build/Release/$bootstrapperBundleName.app", "$assemblyPath/disk_image_source_$installerName/");
    CodeSignBundle("$assemblyPath/disk_image_source_$installerName/$bootstrapperBundleName.app", 1);

    # TODO: Again, sudo is needed here since above root:admin got permissions over all Unity files
    system("chmod -R ugo+rw $assemblyPath/disk_image_source_$installerName");
    $? == 0 or croak "Failed";
    unlink "$assemblyPath/$installerName.dmg";
    BuildDmg("$assemblyPath/disk_image_source_$installerName", "$assemblyPath/$installerName", "$assemblyPath/MacBootstrapper/Template.sparseBundle");

    if ($options->{notarize})
    {
        NotarizeApp("$assemblyPath/$installerName.dmg", "com.unity.DownloadAssistant.dmg.$unityVersion");
    }

    UMove("$assemblyPath/$installerName.dmg", "$root/build/MacBootstrapper/");
}
1;
