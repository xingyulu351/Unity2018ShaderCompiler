package LinuxEditorSelfExtractingShellScript;

use strict;
use warnings;
use Env qw($BZIP2);
use File::Path qw(mkpath rmtree);
use File::Basename qw(dirname);
use File::Spec;
use File::chdir;
use File::Slurp;
use lib File::Spec->rel2abs(dirname($0) . "/../../..") . '/External/Perl/lib';
use OTEE::BuildUtils qw(copy);
use lib File::Spec->rel2abs(dirname($0) . "/..");
use Tools qw (UCopy ProgressMessage ReplaceText SetupPlaybackEngines);

sub Register
{

    Main::RegisterTarget(
        "LinuxEditorSelfExtractingShellScript",
        {
            linuxbuildfunction => \&BuildLinuxEditorSelfExtractingShellScript,
            dependencies => ["linuxeditor"],
            options => { "playbackengines" => ["linuxstandalonesupport"] }
        }
    );
}

sub BuildLinuxEditorSelfExtractingShellScript
{
    my ($root, $targetPath, $options) = @_;
    my $unityVersion = $BuildConfig::unityVersion;
    if ($options->{versionOverride} && $options->{versionOverride} ne "")
    {
        $unityVersion = $options->{versionOverride};
    }
    my $stagingBase = "unity-editor-$unityVersion";
    my $stagingArea = "$targetPath/$stagingBase";
    my $destDir = "$root/build/temp";
    my $playbackEnginePath = "$targetPath/../LinuxEditor/Data/PlaybackEngines/";
    my $licenseText = read_file("$root/Tools/Licenses/License.txt");

    # Inject newline hints because this will be read by less
    $licenseText =~ s/\n/\\n/g;

    # Maximum squeezing!
    $BZIP2 = '-9';

    rmtree($targetPath);
    mkpath($stagingArea);

    SetupPlaybackEngines($root, $options, $playbackEnginePath);

    # Update Unity version and license text in template script
    ReplaceText("$root/Tools/Installers/LinuxEditor/installer.sh", "$targetPath/unity-editor-installer.sh", ("0.0.0f0", $unityVersion));

    ProgressMessage('Deploying to staging area');
    UCopy("$root/build/LinuxEditor", "$stagingArea/Editor");
    UCopy("$root/build/LinuxMonoDevelop", "$stagingArea/MonoDevelop");
    UCopy("$root/Tools/Installers/LinuxEditor/unity-editor.desktop", "$stagingArea/unity-editor.desktop");
    UCopy("$root/Tools/Installers/LinuxEditor/unity-monodevelop.desktop", "$stagingArea/unity-monodevelop.desktop");
    UCopy("$root/build/LinuxEditor/Data/Resources/LargeUnityIcon.png", "$stagingArea/unity-editor-icon.png");

    # Need updated icon
    # UCopy ("$root/build/LinuxMonoDevelop/share/icons/hicolor/48x48/apps/monodevelop.png", "$stagingArea/unity-monodevelop.png");

    ProgressMessage('Building archive');
    chdir($targetPath);
    system("tar cj $stagingBase >> $targetPath/unity-editor-installer.sh");
    chmod(0755, "$targetPath/unity-editor-installer.sh");
}

1;
