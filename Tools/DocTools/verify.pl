#!/usr/bin/env perl

use strict;
use File::Basename qw(dirname basename);
use File::Spec;
use File::Path;
use Cwd;
use Cwd 'abs_path';
use FindBin qw'$Bin';
use lib "$FindBin::Bin/../../External/Perl/lib";
use File::chdir;
use UnTGZPackages;

my $mydir = File::Spec->rel2abs(dirname($0));
my $root = File::Spec->rel2abs(dirname($0) . "/../..");
use lib File::Spec->rel2abs(dirname($0) . "/../..") . '/Tools/Build';
use Tools qw (UCopy XBuild AmRunningOnBuildServer OpenBrowser);
use Tools qw (Jam JamRaw);
use Tools qw(GetMono64Exe);
use Carp qw(croak);
use feature qw(say);

my $config = "Debug";
my $MemValidityProj = "$mydir/MemValidityChecker/MemValidityChecker.csproj";

Jam($root, "DocGen", "", "", 1);

#build MemValidityChecker as a 3.5 assembly
XBuild($root, $MemValidityProj, "Debug", "bin");
$? and die("Problem building MemValidity project");

my $packagetemp = UnTGZPackages();

my $cli = GetMono64Exe();

sub BuildUserDocumentation_MonoDoc
{
    local $CWD = "$mydir/../..";
    my $root = $CWD;
    Jam($root, "DocGen", "", "", 1);
    local $CWD = "$root/Tools/DocTools";
    my $exe = "APIDocumentationGenerator/bin/APIDocumentationGenerator.exe";
    my @monoargs = ($cli, $exe, "-monodoc");
    system(@monoargs) == 0 or die("Failed generating MonoDoc XML files\n");
}

BuildUserDocumentation_MonoDoc();

(chdir "$mydir") == 1 or die("could not change directory to $mydir");
unlink "ApiDocErrors.txt";
system($cli, "MemValidityChecker/bin/Debug/MemValidityChecker.exe") == 0 or die "Error executing MemValidityChecker.exe";
if (-e "ApiDocErrors.txt")
{
    if (AmRunningOnBuildServer())
    {
        die("API doc errors were found");    #stop on Katana
    }
    else
    {
        warn("API doc errors were found");    #continue locally
    }
}

############ScriptRef-only broken links verification###############
my $dbvDir = "$root/build/DocsBuildVerificationScriptRef";

if (rmtree($dbvDir) && !($!{ENOENT}))
{
    die "'$dbvDir' exists but we could not remove it : $!";
}

if (-d $packagetemp)
{
    rmtree($packagetemp) or die("Unable to remove $packagetemp (another process has a lock on it?)\n");
}

#build the no-examples version of the ScriptRef into build/UserDocumentation
my $runresult = system('perl', 'run.pl', '-noexamples');
if ($runresult != 0)
{
    die("Failed to build ScriptRef");
}

# Copy documentation files (Katana prefers to pick up artifacts from one folder)
my $artifacts_folder = "$root/build/UserDocumentation/Documentation/en/ScriptReference/";   # ending slash is required for UCopy to work on both Mac and Windows
my $output_folder = "$dbvDir/ScriptReference/";
say("Copying docs from '$artifacts_folder' to '$output_folder'");
mkdir($output_folder);
UCopy($artifacts_folder, $output_folder);
UCopy("$root/build/UserDocumentation/Documentation/en/StaticFiles/", "$dbvDir/StaticFiles");

# Run link validation
my $output_folder_relative = "../../build/DocsBuildVerificationScriptRef/Linklint";
my $output_folder_absolute = "$dbvDir/Linklint";
$artifacts_folder = "$dbvDir";
mkdir($output_folder_absolute);
my @args = (
    "perl", "linklint/linklint-2.3.5.pl", "\@linklint/ScriptRefVerification.args",
    "-quiet", "/@", "-doc", $output_folder_relative, "-root", "$artifacts_folder", "-docbase", "..", "-no_anchors"
);

if ($^O eq "MSWin32")
{
    push(@args, "-case");
}
print("Running '@args'...\n");
my $result = system(@args);
if ($result != 0)
{
    if (AmRunningOnBuildServer())
    {
        say("See the artifact DocsBuildVerificationScriptRef, path /DocsBuildVerificationScriptRef/Linklint/errorX.html");
    }
    else
    {
        OpenBrowser("file://$output_folder_absolute/index.html");
    }
    croak("ERROR: Link verification failed");
}
else
{
    print "Link verification succeeded\n";
}
