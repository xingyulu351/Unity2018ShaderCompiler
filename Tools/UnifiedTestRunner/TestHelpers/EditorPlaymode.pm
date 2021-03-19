package TestHelpers::EditorPlaymode;

use File::Temp qw (tempdir);
use File::Path qw(make_path);
use File::Spec::Functions qw (canonpath);
use FileUtils qw (openForWriteOrCroak touch);

use Exporter 'import';
our @EXPORT_OK = qw [createEmptyUnityProject createPlaymodeUnityProject createEditmodeUnityProject createPlaymodeAndEditmodeUnityProject];

sub createEmptyUnityProject
{
    my ($root, $name) = @_;
    if (not defined($name))
    {
        ++$projectId;
        $name = "Project$projectId";
    }
    my $p = canonpath("$root/$name/Assets");
    make_path($p);
    my $fh = openForWriteOrCroak("$p/dummy.asset");
    close($fh);
    return canonpath("$root/$name");
}

sub createPlaymodeUnityProject
{
    my ($root, $name) = @_;
    my $projectPath = createEmptyUnityProject($root, $name);

    my $fh = openForWriteOrCroak("$projectPath/Assets/APlayModeTest.cs");
    close($fh);
    make_path("$projectPath/ProjectSettings");
    FileUtils::writeAllLines(
        "$projectPath/ProjectSettings/ProjectSettings.asset", '%YAML 1.1
%TAG !u! tag:unity3d.com,2011:
--- !u!129 &1
PlayerSettings:
  playModeTestRunnerEnabled: 1'
    );
    return $projectPath;
}

sub createEditmodeUnityProject
{
    my ($root, $name) = @_;
    my $projectPath = createEmptyUnityProject($root, $name);
    make_path("$projectPath/Assets/Editor");
    touch("$projectPath/Assets/Editor/AnEditModeTest.cs");
    return $projectPath;
}

sub createPlaymodeAndEditmodeUnityProject
{
    my ($root, $name) = @_;
    my $projectPath = createPlaymodeUnityProject($root, $name);
    make_path("$projectPath/Assets/Editor");
    touch("$projectPath/Assets/Editor/AnEditModeTest.cs");
    return $projectPath;
    close($fh);
    return $projectPath;
}

1;
