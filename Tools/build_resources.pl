#!/usr/bin/perl
use strict;
use warnings;

package Main;
use FindBin qw($Bin);
use File::Basename qw(dirname basename fileparse);
use File::Path qw(mkpath rmtree);
use File::Spec::Functions;
use File::Copy;
use lib ("$Bin/../External/Perl/lib");
use File::chdir;
use File::Copy::Recursive qw(fcopy rcopy dircopy fmove rmove dirmove);
use Time::HiRes qw(time);

my $root = File::Spec->rel2abs(dirname($0) . '/..');

my $buildBuiltin = 0;
my $buildEditor = 0;
my $buildExtra = 0;
my $buildPreviews = 1;
my $zipBuilds = 1;

my $rebuildLibrary = 1;
my $shaderFlags = "";
my $builtinTypeTreeFlags = "";

my $usage = "Usage: build_resources.pl [-builtin] [-editor] [-extra]   [-i] [-noshadercheck] [-nopreviews] [-nozip] [-builtintypetree]\n";

foreach my $arg (@ARGV)
{
    if ($arg eq "-builtin")
    {
        $buildBuiltin = 1;
    }
    elsif ($arg eq "-editor")
    {
        $buildEditor = 1;
    }
    elsif ($arg eq "-extra")
    {
        $buildExtra = 1;
    }
    elsif ($arg eq "-i")
    {
        $rebuildLibrary = 0;
    }
    elsif ($arg eq "-noshadercheck")
    {
        $shaderFlags = "-noshadercheck";
    }
    elsif ($arg eq "-builtintypetree")
    {
        $builtinTypeTreeFlags = "-builtintypetree";
    }
    elsif ($arg eq "-nopreviews")
    {
        $buildPreviews = 0;
    }
    elsif ($arg eq "-nozip")
    {
        $zipBuilds = 0;
    }
    else
    {
        die $usage;
    }
}

if (!($buildBuiltin || $buildExtra || $buildEditor))
{
    $buildBuiltin = 1;
    $buildEditor = 1;
    $buildExtra = 1;
}

if (!$buildBuiltin)
{
    $zipBuilds = 0;
}

my $unityPath;
my $diffPath;
my $zipPath;

if ($^O eq "darwin")
{
    $unityPath = "$root/build/MacEditor/Unity.app/Contents/MacOS/Unity";
    $diffPath = 'diff';
    $zipPath = "$root/External/7z/osx/7za";
}
elsif ($^O eq 'MSWin32')
{
    $unityPath = "$root\\build\\WindowsEditor\\Unity.exe";
    $diffPath = "$root\\External\\Utilities\\Windows\\Diff\\diff.exe";
    $zipPath = "$root\\External\\7z\\win32\\7za.exe";
}
elsif ($^O eq 'linux')
{
    $unityPath = "$root/build/LinuxEditor/Unity";
    $diffPath = 'diff';
    $zipPath = "$root/External/7z/linux64/7za";
}
else
{
    die "Unsupported platform: $^O\n";
}

my $projectPath = "";
my $defaultResourcePath;
my $tempFolder;
my $logPath;
my $cleanLogPath;

sub PrintCleanLog
{
    #@TODO: Read the clean log and ensure that it is empty or has no unexpected error
    open CLEANLOGFILE, "<$cleanLogPath";
    print("\n\n******  Clean Log Output   *****   \n\n\n");
    print <CLEANLOGFILE>;
    print("\n******  LOG END   *****   \n");
    close CLEANLOGFILE;

}

my $baseLogPath = "$root/build/log/resources";
if (!(-d $baseLogPath))
{
    mkpath($baseLogPath);
}

if ($buildEditor)
{
    print("\nBuilding editor resources...\n");
    $projectPath = catfile($root, "External", "Resources", "editor_resources");
    $defaultResourcePath = catfile($root, "Editor", "Resources", "Common", "unity editor resources");
    $tempFolder = catfile($root, "build", "resources", "editor_resources");
    $logPath = catfile($baseLogPath, "editor_resources.log");
    $cleanLogPath = catfile($baseLogPath, "clean_editor_resources.log");

    # Cleanup diff folder and copy over old builtin resource file
    #rmtree($tempFolder);
    #mkpath($tempFolder);
    #copy ($defaultResourcePath, $tempFolder . "/old");

    # build
    #rmtree (catfile($projectPath, "Library")) if ($rebuildLibrary);
    print("\nLaunching Unity in '$unityPath' log '$cleanLogPath' proj '$projectPath'\n");
    system($unityPath, "-cleanedLogFile", $cleanLogPath, "-logFile", $logPath,
        "-batchmode", "-forgetProjectPath", "-nographics", $shaderFlags, $builtinTypeTreeFlags,
        "-force-gfx-direct", "-forceFullStacktrace", "Assert", "-projectpath", $projectPath,
        "-buildEditorUnityResources", "-outputpath", $defaultResourcePath, "-quit"
    );
    die "Failed to build editor resources or launch Unity\n" if $?;

    # copy builtin resource and generate text file
    #copy ($defaultResourcePath, $tempFolder . "/new") or die "Failed copy";

    #diff_resources ("Editor", $tempFolder . '/old', $tempFolder . '/old.txt', $tempFolder . '/new', $tempFolder . '/new.txt');
    PrintCleanLog();
}

if ($buildBuiltin || $buildExtra)
{
    print("\nBuilding player resources...\n");
    $projectPath = "$root/Runtime/Resources";
    my $oldBuiltinResourcesPath = catfile($projectPath, "builds");

    my $buildOutputDir = catfile($root, "build", "resources", "builtin_resources");
    $logPath = catfile($baseLogPath, "builtin_resources.log");
    my $logPathPreviews = catfile($baseLogPath, "builtin_resources_previews.log");
    $cleanLogPath = catfile($baseLogPath, "clean_builtin_resources.log");

    # Cleanup output folder
    rmtree($buildOutputDir);
    mkpath($buildOutputDir);

    # build
    rmtree(catfile($projectPath, "Library")) if ($rebuildLibrary);
    print("\nLaunching Unity in '$unityPath' log '$cleanLogPath' proj '$projectPath'\n");
    my $time0 = time();
    system($unityPath, "-cleanedLogFile", $cleanLogPath,
        "-logFile", $logPath, "-batchmode",
        "-forceFullStacktrace", "Assert", "-forgetProjectPath",
        "-nographics", $shaderFlags, $builtinTypeTreeFlags,
        "-force-gfx-direct", "-projectpath", $projectPath,
        $buildBuiltin ? "-buildBuiltinUnityResources" : "", $buildExtra ? "-buildBuiltinExtras" : "", "-outputpath",
        $buildOutputDir, "-quit"
    );
    die "Failed to build resources or launch Unity\n" if $?;
    my $time1 = time();
    printf("\n  building builtin resources took %.1f seconds\n", $time1 - $time0);

    # verify there were no errors
    open FILE, $cleanLogPath or die "Couldn't open file: $!";
    my $cleanLogContent = join("", <FILE>);
    close FILE;
    if ($cleanLogContent ne "" && $cleanLogContent ne "Rebuilding Library because the asset database could not be found!\n\n")
    {
        print "--$cleanLogContent --";
        die "Failed build because Unity had errors while importing or building.\n";
    }

    if ($buildPreviews)
    {
        my $time0 = time();
        system($unityPath, "-logFile", $logPathPreviews, "-projectpath", $projectPath,
            "-batchmode", "-forceFullStacktrace", "Assert", "-forgetProjectPath", "-buildBuiltinPreviews",
            "-outputpath", $buildOutputDir, "-quit"
        );
        die "Failed to generate builtin previews or launch Unity\n" if $?;
        my $time1 = time();
        printf("\n  building builtin previews took %.1f seconds\n", $time1 - $time0);

        #open(DUMMYMETRO, ">$dummyMetro") && close(DUMMYMETRO);
    }

    if ($zipBuilds)
    {
        #Warn about committing locally-build resources
        print "\n***** DO NOT COMMIT RESOURCES BUILT LOCALLY *****";
        print "\n* BUILTIN RESOURCES BUILT LOCALLY DO NOT WORK WITH CONSOLES. ***";
        print "\n* Build them on Katana instead, otherwise bad things will happen. ***";
        print "\n* DO NOT COMMIT RESOURCES BUILT LOCALLY *\n";

        zip_builds($buildOutputDir);
    }
}

chdir $root;
exit 0;

sub diff_resources
{
    my ($name, $old, $oldTxt, $new, $newTxt) = @_;
    if (!-e $old)
    {
        print "old binary to text file does not exist\n";
        return;
    }
    my $bin2txt = $^O eq 'MSWin32' ? "$root/build/WindowsEditor/Data/Tools/binary2text.exe" : "$root/build/MacEditor/Unity.app/Contents/Tools/binary2text";
    system($bin2txt, $old, $oldTxt);
    die "failed generate diff old.\n" if $?;
    system($bin2txt, $new, $newTxt);
    die "failed generate diff new.\n" if $?;
    print "*** $name resource file diff ***\n\n\n";
    system($diffPath, "-u", $oldTxt, $newTxt);

    print "\n*** $name diff END ***\n";
}

sub unzip_builds
{
    my ($basepath) = @_;
    my $zipfile = "$basepath/builds.zip";
    my $targetdir = "$basepath/builds";
    if (-e $zipfile)
    {
        rmtree $targetdir;
        mkpath $targetdir;
        print "Unzipping $zipfile into $targetdir\n";
        if ($^O eq 'MSWin32')
        {
            system($zipPath, "x", "-o$targetdir", $zipfile) eq 0 or die("Failed to unzip $zipfile\n");
        }
        else
        {
            local $CWD = $targetdir;
            system("unzip", "-q", "$root/$zipfile") eq 0 or die("failed to unzip $zipfile");
        }
    }
}

sub zip_builds
{
    my ($basepath) = @_;

    chdir("$basepath");
    system($zipPath, "a", "builds-xboxone.zip", "resources_xboxone") && die("Failed to compress XboxOne support");
    system($zipPath, "a", "builds-ps4.zip", "resources_ps4") && die("Failed to compress ps4 support");
    system($zipPath, "a", "builds-switch.zip", "resources_switch") && die("Failed to compress Switch support");
    system($zipPath, "a", "builds-lumin.zip", "resources_lumin") && die("Failed to compress Lumin support");

    # Build non-secret excluding all secret files.
    system($zipPath, "a", "builds-non-secret.zip", "-x!resources_xboxone", "-x!resources_ps4", "-x!resources_switch", "-x!resources_lumin", "-x!*.zip")
        && die("Failed to compress non-secret support");

    # Pack final zip
    system($zipPath, "a", "builds-Resources.zip", "*.zip") && die("Failed to Failed to create container archive");

    print("\nCreated builds-Resources.zip.  You must run build build.pl --applyBuildsZip=$basepath/builds-Resources.zip");
    print("\n to put files in correct location.");
}
