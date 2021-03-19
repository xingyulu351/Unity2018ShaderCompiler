use strict;
use File::Basename qw(dirname basename);
use File::Spec;
use Getopt::Long;

use Cwd;
use Cwd 'abs_path';
use Carp qw( croak );
use feature qw(say);

my $doctoolsdir = File::Spec->rel2abs(dirname($0));
my $root = File::Spec->rel2abs(dirname($0) . "/../..");
use lib File::Spec->rel2abs(dirname($0) . "/../..") . '/Tools/Build';
use Tools qw (UCopy Jam AmRunningOnBuildServer);
use Unity::XBuild qw(RunCSProj);

my $doupload = 0;
my $cleanzipfilename = 0;
my $help = 0;
Getopt::Long::GetOptions(
    'cleanzipfilename' => \$cleanzipfilename,
    'doupload' => \$doupload,
    'help' => \$help
);

if ($help)
{
    print("Options:\n");
    print("  --cleanzipfilename          Names scriptRef package without branch name.\n");
    print("  --doupload           Upload the zip file to the translate server.\n");
    exit 0;
}

my $config = "Debug";
my $htmlGenProj = "$doctoolsdir/APIDocumentationGenerator/APIDocumentationGenerator.csproj";
my $modelProj = "$doctoolsdir/UnderlyingModel/UnderlyingModel.csproj";

my $editorProj = "$root/Projects/CSharp/UnityEditor.csproj";

my $parserSln = "$doctoolsdir/DocTools.sln";

Jam($root, "DocGen", "", "", 1);

my $SevenZip;
if ($^O eq 'MSWin32')
{
    $SevenZip = "$root/Tools/WinUtils/7z/7z.exe";
}
else
{
    $SevenZip = "$root/Tools/MacUtils/7za";
}

unlink("$doctoolsdir/ScriptRefPackage.zip");

chdir("$root") == 1 or die("could not change directory to $root");

my $name = "ScriptRefPackage.zip";
if (!$cleanzipfilename)
{
    my $branch = `hg branch`;
    chomp($branch);
    $branch =~ s/\//\-/g;
    $name = "ScriptRefPackage-$branch.zip";
}

unlink "$doctoolsdir/$name";
system("$SevenZip", "a", "-r", "$doctoolsdir/$name", $^O eq 'MSWin32' ? "\@$doctoolsdir/listfile.txt" : "\@$doctoolsdir/listfile.mac.txt") == 0
    or croak("Failed to zip up the ScriptRef Package");

chdir("$doctoolsdir") == 1 or die("could not change directory to $doctoolsdir\n");

if ($doupload)
{
    system("pscp -l translate $name translate-prod.hq.unity3d.com:/home/translate/incoming/") and die("couldn't transfer the zip file");
}
