package LinuxEditorDebPackage;

use strict;
use warnings;
use Env qw(@LD_LIBRARY_PATH $DESTDIR $UNITY_SOURCE $MONODEVELOP_SOURCE);
use File::Path qw(mkpath rmtree);
use File::Basename qw(dirname basename);
use File::Spec;
use File::chdir;
use lib File::Spec->rel2abs(dirname($0) . "/../../..") . '/External/Perl/lib';
use OTEE::BuildUtils qw( copy);
use lib File::Spec->rel2abs(dirname($0) . "/..");
use Tools qw (UCopy ProgressMessage ReplaceText SetupPlaybackEngines);

sub Register
{

    Main::RegisterTarget(
        "LinuxEditorDebPackage",
        {
            linuxbuildfunction => \&BuildLinuxEditorDebPackage,
            dependencies => ["linuxeditor"],
            options => { "playbackengines" => ["linuxstandalonesupport"] }
        }
    );
}

sub WriteEULAToConfigFile
{
    my ($root, $templateFile) = @_;
    my $licenseFile = "$root/Tools/Licenses/License.txt";
    open(LICENSE, '<', $licenseFile) or croak("Unable to open license file at $licenseFile\n");
    open(TEMPLATE, '>>', $templateFile) or croak("Unable to open template file at $templateFile\n");
    foreach my $line (<LICENSE>)
    {
        if ($line =~ /^\s*$/)
        {
            print(TEMPLATE " .\n");
        }
        else
        {
            print(TEMPLATE " $line");
        }
    }
    close(LICENSE);
    close(TEMPLATE);
}

sub BuildLinuxEditorDebPackage
{
    my ($root, $targetPath, $options) = @_;
    my $unityVersion = $BuildConfig::unityVersion;
    if ($options->{versionOverride} && $options->{versionOverride} ne "")
    {
        $unityVersion = $options->{versionOverride};
    }
    my $stagingArea = "$targetPath/unity-editor-$unityVersion";
    my $destDir = "$root/build/temp";
    my $playbackEnginePath = "$targetPath/../LinuxEditor/Data/PlaybackEngines/";

    rmtree($targetPath);
    mkpath($targetPath);
    mkpath($stagingArea);
    rmtree($destDir);
    mkpath($destDir);

    SetupPlaybackEngines($root, $options, $playbackEnginePath);

    UCopy("$root/Tools/Installers/LinuxEditor/debian", "$stagingArea/debian");
    UCopy("$root/Tools/Installers/LinuxEditor/Makefile", "$stagingArea/Makefile");
    UCopy("$root/Tools/Installers/LinuxEditor/unity-editor.desktop", "$stagingArea/unity-editor.desktop");
    UCopy("$root/Tools/Installers/LinuxEditor/unity-monodevelop.desktop", "$stagingArea/unity-monodevelop.desktop");

    ReplaceText("$stagingArea/debian/changelog", "$stagingArea/debian/changelog", ("0.0.0f0", $unityVersion));
    ReplaceText("$stagingArea/debian/files", "$stagingArea/debian/files", ("0.0.0f0", $unityVersion));
    ReplaceText("$stagingArea/unity-editor.desktop", "$stagingArea/unity-editor.desktop", ("0.0.0f0", $unityVersion));

    WriteEULAToConfigFile($root, "$stagingArea/debian/templates");

    push(@LD_LIBRARY_PATH, "$root/build/LinuxEditor/Data/Tools");
    $DESTDIR = $destDir;
    $UNITY_SOURCE = "$root/build/LinuxEditor";
    $MONODEVELOP_SOURCE = "$root/build/LinuxMonoDevelop";
    chdir("$stagingArea");
    system("dpkg-buildpackage", "-b");
    system("mv", "$targetPath/unity-editor_" . $unityVersion . "_amd64.deb", "$targetPath/unity-editor_amd64.deb");
}

1;
