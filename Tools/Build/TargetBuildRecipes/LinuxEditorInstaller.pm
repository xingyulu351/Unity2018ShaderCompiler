package LinuxEditorInstaller;

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
use Tools qw (SetupPlaybackEngines UCopy UMove AmRunningOnBuildServer ProgressMessage ReplaceText ProgressMessage Jam);
use Digest::MD5;
use Time::localtime;
use UserDocumentation;
use TargetSupportInstaller;

my $assemblyPath = "build/temp";
my $unityAppPath = "build/LinuxEditor/Unity";

use BuildConfig;
our $unityVersion = $BuildConfig::unityVersion;
our $copyright = $BuildConfig::copyright;

sub Register
{
    Main::RegisterTarget(
        "LinuxEditorInstallerSet",
        {
            linuxbuildfunction => \&BuildLinuxEditorInstallerSet,
            dependencies => ["LinuxBootstrapper", "LinuxEditorInstaller",],
        }
    );

    Main::RegisterTarget(
        "LinuxEditorInstaller",
        {
            linuxbuildfunction => \&BuildLinuxEditorInstaller,
            dependencies => ["LinuxEditor", "LinuxStandaloneSupport",],
            options => {
                "codegen" => ["debug", "release"],
                "buildSudoPass" => [0, 1],
                "incremental" => [0, 1],
                "playbackengines" => ["LinuxStandaloneSupport"]
            }
        }
    );

    Main::RegisterTarget(
        "LinuxEditorTargetInstaller",
        {
            linuxbuildfunction => \&TargetSupportInstaller::BuildTargetSupportInstallerLinux,
            options => { "playbackengines" => ["iOSSupport"] }
        }
    );

    Main::RegisterTarget(
        "LinuxBootstrapper",
        {
            linuxbuildfunction => \&BuildLinuxBootstrapper,
            options => {
                "codegen" => ["debug", "release"],
                "incremental" => [0, 1],
            }
        }
    );
}

sub BuildLinuxEditorInstallerSet
{
    my ($root, $targetPath, $options) = @_;
    $assemblyPath = "$root/$assemblyPath";

    my %svnInfo = svnGetRevision();

    # At this point, this script expects to have all dependencies,
    # at the following locations:
    #
    # Unity: build/LinuxEditorInstaller/Unity.tar.xz

    # Generate the bootstrapper ini file
    my @modules = (
        {
            "name" => "Unity",
            "build" => "$root/build/LinuxEditorInstaller/Unity.tar.xz",
            "description" => "Unity Editor",
            "url" => "LinuxEditorInstaller/Unity.tar.xz",
            "mandatory" => "false",
            "install" => "true",
            "version" => $unityVersion,
        },
    );

    if (AmRunningOnBuildServer())
    {
        my @targets = (

            # LinuxEditorInstaller contains the Linux Standalone,
            # but keeping this for reference if we want to ship it separately
            # {name=>'Linux'},
            { name => 'Android' },

            { name => 'iOS' },

            # { name => 'AppleTV' },
            { name => 'Mac-Mono' },

            # { name => 'Vuforia-AR' },
            { name => 'WebGL' },
            { name => 'Windows-Mono' },
            { name => 'Facebook-Games' },
        );

        my @targets_enabled = ('Linux');

        # We reuse the target support installers from mac, except iOS
        foreach my $target (@targets)
        {
            my $tname = $target->{name};
            my $url = "MacEditorTargetInstaller/UnitySetup-$tname-Support-for-Editor-$unityVersion.pkg";
            my $path = "$root/build/$url";
            if (!-e $path)
            {
                $url = "LinuxEditorTargetInstaller/UnitySetup-$tname-Support-for-Editor-$unityVersion.tar.xz";
                $path = "$root/build/$url";
            }
            die("Required file not found: $path\n") unless (-e $path);

            my $install = (grep { $_ eq $tname } @targets_enabled) ? 'true' : 'false';

            my $module = {
                "name" => $tname,
                "build" => $path,
                "description" => "Allows building your Unity projects for the $tname platform",
                "url" => $url,
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

    my $inifile = "$root/build/LinuxEditorInstallerSet/$BuildConfig::bootstrapperIniFileLinux";
    open my $INI, ">$inifile" or croak "Failed to open $inifile";
    for my $module (@modules)
    {
        my $ctx = Digest::MD5->new;
        open my $pkgfh, "<$module->{build}" or croak("Failed to open '$module->{build}' for module '$module->{name}'");
        $ctx->addfile($pkgfh);
        close $pkgfh;

        my $md5 = $ctx->hexdigest;
        my $size = -s $module->{build};

        my $installedsize = '';
        if ($module->{build} =~ /\.pkg$/)
        {
            my $pkginfo = `7z x $module->{build} '*/PackageInfo' -so | tee`;
            if ($?)
            {
                croak "7z failed on file: $module->{build}" if $?;
            }

            $pkginfo =~ /installKBytes="([^"]*)"/ or croak "Failed to get installed size of $module->{build}";
            $installedsize = int($1) * 1000;
        }
        elsif ($module->{build} =~ /\.tar\.xz$/)
        {
            my $pkginfo = `tar taf $module->{build} --totals 2>&1 | tail -1`;
            if ($?)
            {
                croak "tar failed on file: $module->{build}" if $?;
            }

            $pkginfo =~ /Total bytes read: (\d+)/ or croak "Failed to get installed size of $module->{build}";
            $installedsize = int($1);
        }

        my $title = $BuildConfig::installerPackageTitles{ $module->{name} };
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
        my $ini_sec =
            "[$module->{name}]\ntitle=$title\ndescription=$module->{description}\nurl=$module->{url}\nmd5=$md5\ninstall=$module->{install}\nmandatory=$module->{mandatory}\nsize=$size\ninstalledsize=$installedsize\n$version$requires_unity$sync";
        map { $ini_sec .= "$_=$module->{$_}\n" } grep /^optsync_/, keys %{$module};
        print $INI $ini_sec;
    }
    close $INI;

    # Copy download.html and substitute variables
    open(my $download_html_in, "<$root/Tools/Installers/LinuxBootstrapper/download.html") or die $!;
    open(my $download_html_out, ">$root/build/LinuxEditorInstallerSet/download.html") or die $!;

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

sub BuildLinuxEditorInstaller
{
    my ($root, $targetPath, $options) = @_;

    $assemblyPath = "$root/$assemblyPath";
    my $targetFile = 'Unity.tar.xz';

    ProgressMessage("Building Linux Editor - $targetFile - Unity");

    rmtree("$root/build/LinuxEditorInstaller");
    mkpath("$root/build/LinuxEditorInstaller");

    rmtree($assemblyPath);
    mkpath($assemblyPath);

    ProgressMessage("Deploying LinuxEditor to $assemblyPath");
    UCopy("$root/build/LinuxEditor", "$assemblyPath/Editor");

    UserDocumentation::BuildUserDocumentation_MonoDoc($assemblyPath);

    ## Process dependencies
    my $playbackEnginePath = "$assemblyPath/Editor/Data/PlaybackEngines";
    SetupPlaybackEngines($root, $options, $playbackEnginePath);

    ProgressMessage("Adding Plugin Interface Header Files");
    rmtree("$root/build/LinuxEditor/Data/PluginAPI");
    UCopy("$root/Runtime/PluginInterface/Headers", "$root/build/LinuxEditor/Data/PluginAPI");

    ProgressMessage("Creating installer archive");
    system('tar', 'cJf', "$targetPath/$targetFile", '--directory', "$assemblyPath", 'Editor') == 0 or croak("Failed to zip up artifacts");
}

sub BuildLinuxBootstrapper
{
    my ($root, $targetPath, $options) = @_;
    my %svnInfo = svnGetRevision();
    my $svnRevision = "$svnInfo{Revision}";

    chdir($root);
    rmtree($targetPath);
    mkpath($targetPath);

    $unityAppPath = "$root/$unityAppPath";

    # Need to generate this before building the bootstrapper
    mkpath("$root/artifacts/generated/LinuxBootstrapper");
    my $iniHeader = "$root/artifacts/generated/LinuxBootstrapper/settings.ini.h";
    open my $INI, ">$iniHeader" or die "Failed to open $iniHeader";
    print $INI "const char* g_SettingsIni = \"[settings]\\n\"\n";
    my $mirroridx = 1;
    foreach my $iniloc (@BuildConfig::bootstrapperIniLocations)
    {
        print $INI "\"ini_url$mirroridx=$iniloc/$svnRevision/$BuildConfig::bootstrapperIniFileLinux\\n\"\n";
        $mirroridx++;
    }
    print $INI "\"\";\n";
    close $INI;

    if (AmRunningOnBuildServer() || exists $ENV{'UNITY_USE_LINUX_SDK'})
    {
        ProgressMessage('Setting up the Linux SDK');
        SDKDownloader::PrepareSDK('linux-sdk', '20180406', "$root/artifacts");
    }

    Jam($root, "LinuxBootstrapper", $options->{codegen}, 'linux64', $options->{incremental});

    if (AmRunningOnBuildServer())
    {
        chdir("$root/build/LinuxBootstrapper");
        system("$root/External/7z/linux64/7za", "a", "-r", 'LinuxBootstrapper.zip', "*") == 0 or croak("Failed to zip up artifacts");
        chdir($root);
        UMove("$root/build/LinuxBootstrapper/LinuxBootstrapper.zip", "$targetPath/");
    }
}
1;
