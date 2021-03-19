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
use Tools qw (UCopy XBuild NewXBuild XBuild4 AmRunningOnBuildServer);
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

#build underlying model as a 3.5 assembly
NewXBuild($root, $modelProj, "$config", "bin/$config");
$? and die("Problem building Underlying model");

XBuild($root, $editorProj, "$config", "bin/$config");
$? and die("Problem building Editor Project");

#build reassembler as a 4.0 assembly
XBuild4($root, $htmlGenProj, "$config", "bin");
$? and die("Problem building HtmlGen project");
