###########################################################################################################
# How to use:
# Drop all the Enlighten SDK zip files into a folder and use this script to convert them to builds.zip's.
#
# Enlighten SDK file naming convention is:
# EnlightenSDK-2.21.Unity.P2.30068.zip for main SDK.
# EnlightenSDK_OSX-2.21.Unity.P2.30068 for per platform libs/includes/etc.
###########################################################################################################
use warnings;
use strict;
use Getopt::Long;
use Cwd;
use File::Spec;
use File::Spec::Functions;
use File::Copy;
use File::Path qw (rmtree mkpath);
use File::Basename qw (dirname fileparse);
use FindBin qw($Bin);
use lib ("$Bin/../../External/Perl/lib");
use File::chdir;
use File::Copy::Recursive qw(dircopy);

my $sdkPrefix = "EnlightenSDK";
my $zip = ".zip";
my $builds = "builds";
my $root = getcwd();

my ($sdkVersion, $sourceDirectory);
GetOptions("version=s" => \$sdkVersion, "sourceDir=s" => \$sourceDirectory);

###########################################

sub CopyDirectoryFiltered
{
    # Copy all the files in the directory except the filtered file extensions.
    my ($source, $dest) = (@_);

    if (-e $source)
    {
        print "source $source\n";
        print "dest $dest\n";
        mkpath($dest);
    }
    else
    {
        print "$source does not exist\n";
        die();
    }

    # Make a filter file - perl regex magic could probably replace all this with only a few lines of code.
    my $filterFileName = "excludes.txt";
    unlink($filterFileName);
    open(my $fh, '>', $filterFileName);
    print $fh ".cpp\n";
    print $fh ".xml\n";
    print $fh ".vcxproj\n";
    print $fh ".vcxproj.filters\n";
    print $fh ".effects\n";
    print $fh ".ceffects\n";
    print $fh ".fpo\n";
    print $fh ".vpo\n";
    print $fh ".mesh\n";
    print $fh ".png\n";
    print $fh "ue4\n";
    print $fh "UE4\n";
    close $fh;

    # Strip trailing backslash
    $source =~ s/\\$//;

    #copy stuff
    system("xcopy", "$source", "$dest", "/S/Y/EXCLUDE:$sourceDirectory\\$filterFileName") eq 0
        or die("failed to xcopy $source to $dest. Args: /S/Y/EXCLUDE:$sourceDirectory\\$filterFileName");
}

###########################################

sub CopyDirectory
{
    my ($source, $dest) = (@_);
    if (-e $source)
    {
        print "source $source\n";
        print "dest $dest\n";
        mkpath($dest);
        dircopy($source, $dest) or die();
    }
    else
    {
        print "$source does not exist\n";
        die();
    }
}

###########################################

sub ClearBuildsDir()
{
    my $tp = "$builds";
    rmtree($tp);
    mkpath($tp);
}

###########################################

if (not $sdkVersion)
{
    print "Missing version! Pass in -version=2.21.Unity.P2.30068 if your main version is EnlightenSDK-2.21.Unity.P2.30068.zip\n";
    print "Usage: prepareEnlightenSDK.pl -version=... -sourceDir=source directory\n";
    die();
}
else
{
    print("Version is $sdkPrefix-$sdkVersion\n");
}

if (not $sourceDirectory)
{
    $sourceDirectory = getcwd();
}
print "Source directory is $sourceDirectory\n";
chdir($sourceDirectory);

ClearBuildsDir();    # clean builds folder

my $outputDir = "$sourceDirectory\\$builds";

###########################################

sub ExtractBaseSDK
{
    # Extract base SDK for the others to use.
    my $baseSDK = "$sourceDirectory\\$sdkPrefix-$sdkVersion";
    if (-e "$baseSDK$zip")
    {
        DecompressZipFile("$baseSDK$zip", $baseSDK);
    }
    else
    {
        print("Failed. Could not find $baseSDK$zip.\n");
        die();
    }
}

sub SaveVersionFile
{
    # Save a version file
    my $versionFileName = "$outputDir\\version.txt";
    open(my $fh, '>', $versionFileName);
    print $fh "Version is $sdkPrefix-$sdkVersion\n";
}

print "root directory is $root\n";

###########################################

# Editor and players for Windows, Mac and Linux
{
    ClearBuildsDir();
    ExtractBaseSDK();

    my $baseSDK = "$sourceDirectory\\$sdkPrefix-$sdkVersion";
    CopyDirectory("$baseSDK\\Src\\EnlightenAPI\\Include\\", "$outputDir\\Include\\");
    CopyDirectoryFiltered("$baseSDK\\Src\\EnlightenAPI\\LibSrc\\", "$outputDir\\LibSrc\\");

    # Extract Windows libs
    my $windowsExtraSDK = "$sourceDirectory\\$sdkPrefix" . "_WindowsExtra-" . $sdkVersion;
    if (-e "$windowsExtraSDK$zip")
    {
        DecompressZipFile("$windowsExtraSDK$zip", $windowsExtraSDK);
        CopyDirectory("$windowsExtraSDK\\Src\\EnlightenAPI\\Lib\\WIN32_2015ST\\", "$outputDir\\Lib\\WIN32_2015ST\\");
        CopyDirectory("$windowsExtraSDK\\Src\\EnlightenAPI\\Lib\\WIN64_2015ST\\", "$outputDir\\Lib\\WIN64_2015ST\\");
    }
    else
    {
        print("Warning: Could not find $windowsExtraSDK$zip.\n");
    }

    # Extract OSX libs
    my $OSXSDK = "$sourceDirectory\\$sdkPrefix" . "_OSX-" . $sdkVersion;
    if (-e "$OSXSDK$zip")
    {
        DecompressZipFile("$OSXSDK$zip", $OSXSDK);
        DecompressZipFile("$OSXSDK\\OSX-Release.zip", "$OSXSDK\\OSX-Release");
        CopyDirectory("$OSXSDK\\OSX-Release\\", "$outputDir\\Lib\\OSX-Release\\");
    }
    else
    {
        print("Warning: Could not find $OSXSDK$zip.\n");
    }

    # Extract Linux libs
    my $LinuxSDK = "$sourceDirectory\\$sdkPrefix" . "_Linux-" . $sdkVersion;
    if (-e "$LinuxSDK$zip")
    {
        DecompressZipFile("$LinuxSDK$zip", $LinuxSDK);
        CopyDirectory("$LinuxSDK\\Src\\EnlightenAPI\\Lib\\LINUX32_GCC\\", "$outputDir\\Lib\\LINUX32_GCC\\");
        CopyDirectory("$LinuxSDK\\Src\\EnlightenAPI\\Lib\\LINUX64_GCC\\", "$outputDir\\Lib\\LINUX64_GCC\\");
    }
    else
    {
        print("Warning: Could not find $LinuxSDK$zip.\n");
    }
    SaveVersionFile();
    my $buildsZip = "builds_win-mac-linux.7z";
    CompressZipFile("$outputDir\\", $buildsZip);

    copy($buildsZip, "$root\\External\\Enlighten\\builds.7z");
}

# Android
my $AndroidSDK = "$sourceDirectory\\$sdkPrefix" . "_Android-" . $sdkVersion;
if (-e "$AndroidSDK$zip")
{
    ClearBuildsDir();

    DecompressZipFile("$AndroidSDK$zip", $AndroidSDK);
    CopyDirectory("$AndroidSDK\\Src\\EnlightenAPI\\Lib\\ANDROID\\", "$outputDir\\Lib\\armeabi-v7a\\");
    CopyDirectory("$AndroidSDK\\Src\\EnlightenAPI\\Lib\\ANDROID_INTEL\\", "$outputDir\\Lib\\x86\\");
    SaveVersionFile();
    my $buildsZip = "builds_android.zip";
    CompressZipFile("$outputDir\\", $buildsZip);

    copy($buildsZip, "$root\\PlatformDependent\\AndroidPlayer\\External\\Enlighten\\builds.zip");
}

# iOS
my $iOSSDK = "$sourceDirectory\\$sdkPrefix" . "_IOS-" . $sdkVersion;
if (-e "$iOSSDK$zip")
{
    ClearBuildsDir();

    DecompressZipFile("$iOSSDK$zip", $iOSSDK);
    DecompressZipFile("$iOSSDK\\IOS_LIBSTDCXX-Release.zip", "$iOSSDK\\IOS_LIBSTDCXX-Release");
    CopyDirectory("$iOSSDK\\IOS_LIBSTDCXX-Release\\", "$outputDir\\Lib\\IOS_LIBSTDCXX-Release\\");
    SaveVersionFile();
    my $buildsZip = "builds_ios.zip";
    CompressZipFile("$outputDir\\", $buildsZip);

    copy($buildsZip, "$root\\PlatformDependent\\iPhonePlayer\\External\\Enlighten\\builds.zip");
}

# PS4
my $PS4SDK = "$sourceDirectory\\$sdkPrefix" . "_PS4-" . $sdkVersion;
if (-e "$PS4SDK$zip")
{
    ClearBuildsDir();
    ExtractBaseSDK();

    DecompressZipFile("$PS4SDK$zip", $PS4SDK);
    CopyDirectory("$PS4SDK\\Src\\EnlightenAPI\\Include\\Enlighten3\\", "$outputDir\\Include\\Enlighten3\\");
    CopyDirectoryFiltered("$PS4SDK\\Src\\EnlightenAPI\\LibSrc\\Enlighten3HLRT\\", "$outputDir\\LibSrc\\Enlighten3HLRT\\");
    CopyDirectoryFiltered("$PS4SDK\\Src\\EnlightenAPI\\LibSrc\\GeoBase\\", "$outputDir\\LibSrc\\GeoBase\\");
    CopyDirectoryFiltered("$PS4SDK\\Src\\EnlightenAPI\\LibSrc\\GeoCore\\", "$outputDir\\LibSrc\\GeoCore\\");
    CopyDirectory("$PS4SDK\\Src\\EnlightenAPI\\Lib\\PS4_3500\\", "$outputDir\\Lib\\PS4_3500\\");
    SaveVersionFile();
    my $buildsZip = "builds_ps4.zip";
    CompressZipFile("$outputDir\\", $buildsZip);

    copy($buildsZip, "$root\\PlatformDependent\\PS4\\External\\Enlighten\\builds.zip");
}

# XboxOne
my $XboxOneSDK = "$sourceDirectory\\$sdkPrefix" . "_XboxOne-" . $sdkVersion;
if (-e "$XboxOneSDK$zip")
{
    ClearBuildsDir();
    ExtractBaseSDK();

    DecompressZipFile("$XboxOneSDK$zip", $XboxOneSDK);
    CopyDirectory("$XboxOneSDK\\Src\\EnlightenAPI\\Include\\Enlighten3\\", "$outputDir\\Include\\Enlighten3\\");
    CopyDirectoryFiltered("$XboxOneSDK\\Src\\EnlightenAPI\\LibSrc\\Enlighten3HLRT\\", "$outputDir\\LibSrc\\Enlighten3HLRT\\");
    CopyDirectoryFiltered("$XboxOneSDK\\Src\\EnlightenAPI\\LibSrc\\GeoBase\\", "$outputDir\\LibSrc\\GeoBase\\");
    CopyDirectoryFiltered("$XboxOneSDK\\Src\\EnlightenAPI\\LibSrc\\GeoCore\\", "$outputDir\\LibSrc\\GeoCore\\");

    # Default to March 2016 XDK and VS2012 compiler.
    CopyDirectory("$XboxOneSDK\\Src\\EnlightenAPI\\Lib\\XBOXONE_2016-03-QFE1-2012\\", "$outputDir\\Lib\\XBOXONE-MONO\\");
    SaveVersionFile();
    my $buildsZip = "builds_xboxone.zip";
    CompressZipFile("$outputDir\\", $buildsZip);

    copy($buildsZip, "$root\\PlatformDependent\\XboxOne\\External\\Enlighten\\builds.zip");
}

###########################################

sub CompressZipFile
{
    my ($directoryToCompress, $fileName) = (@_);

    # Delete old zip
    unlink($fileName);

    # Add all the files in the directory.
    my @files = ();
    opendir(DIR, $directoryToCompress) or die $!;
    while (my $file = readdir(DIR))
    {
        # Use a regular expression to ignore files beginning with a period
        next if ($file =~ m/^\./);

        print "Will add: $file\n";
        push @files, "$directoryToCompress\\$file";
    }
    closedir(DIR);
    my @args = ("a");
    if ($fileName =~ m/7z$/)
    {
        # use Zstd compressor with 19 level; slightly larger than LZMA at level 5, but decompresses 5x faster
        # (LZMA takes 25s, Zstd takes 5s)
        push @args, "-mm=ZSTD";
        push @args, "-mx=19";
    }
    system("$root/External/7z/win64/7za.exe", @args, "$fileName", @files) && die("Failed to compress");
}

###########################################

sub DecompressZipFile
{
    my ($fileName, $outputDirectory) = (@_);
    if (-e $fileName)
    {
        print "Extracting $fileName\n";
        if ($^O eq "MSWin32")
        {
            system("$root/External/7z/win64/7za.exe", "x", "-aoa", "-o$outputDirectory", "$fileName") eq 0 or die("Failed to extract $fileName\n");
        }
        else
        {
            system("unzip", "-q", "$fileName", "-d", $fileName) eq 0 or die("failed to extract $fileName");
        }
    }
    else
    {
        print "Could not find $fileName\n";
        die();
    }
}

###########################################

print("Finished without errors.\n");
print("Remember to bump kGISceneManagerToolVersion in Editor\\Src\\GI\\GISceneManagerToolVersion.h\n");
