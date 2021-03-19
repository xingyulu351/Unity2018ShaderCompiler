#
# This package is designed to provide functionality that will apply a builds.zip container file to the appropriate locations.
# It is intended to be invoked via build.pl with:
#
#     perl build.pl --applyBuildsZip=/path/to/builds-Library.zip
#
# It is expected that the passed in ($pathToBuildsZip) is a container file with a name like 'builds-RakNet.zip' (or whatever
# the appropriate library name is).
#
# Inside there should be multiple files with the following naming schemes:
#
#   - builds-ps4.zip -- this will end up at PlatformDependent/PS4/External/LibraryName/builds.zip
#   - builds-xboxone.zip -- this will end up at PlatformDependent/XboxOne/External/LibraryName/builds.zip
#   - builds-android.zip -- this will end up at PlatformDependent/AndroidPlayer/External/LibraryName/builds.zip
#   - builds-iOS.zip -- this will end up at PlatformDependent/iPhonePlayer/External/LibraryName/builds.zip
#   - builds-webgl.zip -- this will end up at PlatformDependent/WebGL/External/LibraryName/builds.zip
#   - builds-metro.zip -- this will end up at PlatformDependent/MetroPlayer/External/LibraryName/builds.zip
#   - builds-winrt.zip -- this will end up at PlatformDependent/WinRT/External/LibraryName/builds.zip
#   - builds-switch.zip -- this will end up at PlatformDependent/Switch/External/LibraryName/builds.zip
#   - builds-non-secret.zip (legacy name for "non-secret" platforms) -- this will end up at External/LibraryName/builds.zip)
#   - builds.zip (for desktop platforms) -- this will end up at External/LibraryName/builds.zip)
#
# In addition to .zip, the extensions .7z and .tar.gz are also supported.
#
# Built-in resources zip is deployed to a slightly different location:
#   - Runtime/Resources/builds.zip for non-secret platforms
#   - PlatformDependent/***/Runtime/Resources/builds.zip for other platforms.
#
# After the devleoper has applied the builds-LibraryName.zip file with this tool, it is up to the devleoper to commit the result and
# verify the builds on TeamCity.

package BuildsZipHelper;

use warnings;
use strict;
use Carp;
use File::Basename qw (dirname fileparse);
use File::Path qw (rmtree mkpath);
use Cwd qw (realpath);
use Tools qw (UCopy);

require Exporter;
our @ISA = qw (Exporter);
our @EXPORT_OK = qw (DeployBuildsZipsToCorrectLocation);

my $root = realpath(File::Spec->rel2abs(dirname(__FILE__)) . '/../..');

my @types = ("Mono", "MonoBleedingEdge", "MonoConsolesStandalone", "MonoMetro", "PhysX", "Umbra", "Resources");
my @platforms = ("non-secret");

sub DeployBuildsZipsToCorrectLocation
{
    my ($pathToBuildsZip) = @_;
    my $type = "";

    # First, check if the builds.zip is valid
    if (not(-e $pathToBuildsZip))
    {
        croak("Did not find file at $pathToBuildsZip!\n");
    }

    # Next, figure out which file we are processing
    my ($filename, $path, $suffix) = fileparse($pathToBuildsZip, ("zip", "7z", "tar.gz")) or croak("Could not parse filename from $pathToBuildsZip!\n");
    $filename =~ s/\.$//;

    print("Attempting to apply $filename.$suffix...\n");

    # Extract the file
    my $targetdir = "$root/temp/$filename";
    rmtree($targetdir);
    mkpath($targetdir);

    if ($suffix eq "zip")
    {
        if ($^O eq "MSWin32")
        {
            system("$root/External/7z/win64/7za.exe", "x", "-aoa", "-bd", "-ba", "-o$targetdir", $pathToBuildsZip) eq 0
                or croak("Failed to extract $filename.$suffix\n");
        }
        else
        {
            system("unzip", "-q", "$pathToBuildsZip", "-d", $targetdir) eq 0 or croak("failed to extract $filename.$suffix");
        }
    }
    elsif ($suffix eq "7z")
    {
        if ($^O eq "MSWin32")
        {
            system("$root/External/7z/win64/7za.exe", "x", "-aoa", "-bd", "-ba", "-o$targetdir", $pathToBuildsZip) eq 0
                or croak("Failed to extract $filename.$suffix\n");
        }
        elsif ($^O eq "darwin")
        {
            system("chmod +x '$root/External/7z/osx/7za'");
            system("$root/External/7z/osx/7za", "x", "-bd", "-ba", "$pathToBuildsZip", "-o$targetdir") eq 0 or croak("Failed to extract $filename.$suffix\n");
        }
        else
        {
            system("$root/External/7z/linux64/7za", "x", "-bd", "-ba", "-o$targetdir", $pathToBuildsZip) eq 0 or croak("Failed to extract $filename.$suffix\n");
        }
    }
    elsif ($suffix eq "tar.gz")
    {
        if ($^O eq "MSWin32")
        {
            system("$root/External/7z/win64/7za.exe", "x", "-bd", "-ba", "-o$targetdir", $pathToBuildsZip) eq 0
                or croak("Failed to decompress $filename.$suffix\n");
            system("$root/External/7z/win64/7za.exe", "x", "-bd", "-ba", "-o$targetdir", "$targetdir/filename.tar") eq 0
                or croak("Failed to extract $filename.$suffix\n");
            unlink("$targetdir/filename.tar");
        }
        else
        {
            system("tar", "-xf", "$pathToBuildsZip", "-C", $targetdir) eq 0 or croak("failed to extract $filename.$suffix");
        }
    }

    print("Successfully extracted $filename.$suffix.\n");

    # Get a list of the contained files and copy them to the relevant places
    my @files = <$targetdir/*>;
    my $file = "";
    my $libraryName = $filename;
    $libraryName =~ s/builds-//;
    my $unsupported = 0;
    foreach $file (@files)
    {
        my ($platformFilename, $platformPath, $platformSuffix) = fileparse($file, ("zip", "7z", "tar.gz")) or croak("Could not parse filename from $file!\n");
        $platformFilename =~ s/\.$//;

        my $isResources = $file =~ /Resources/;
        my $destSubDir = $isResources ? "Runtime/Resources" : "External/$libraryName";

        if ($platformFilename eq "builds")
        {
            my $dir = "$root/$destSubDir/$platformFilename.$platformSuffix";
            print("Found archive for desktop platforms, applying to $dir...\n");
            UCopy($file, $dir);
        }
        elsif ($platformFilename =~ /ps4/)
        {
            $platformFilename =~ s/-ps4//;
            my $dir = "$root/PlatformDependent/PS4/$destSubDir/$platformFilename.$platformSuffix";
            print("Found archive for PS4, applying to $dir...\n");
            UCopy($file, $dir);
        }
        elsif ($platformFilename =~ /xboxone/)
        {
            $platformFilename =~ s/-xboxone//;
            my $dir = "$root/PlatformDependent/XboxOne/$destSubDir/$platformFilename.$platformSuffix";
            print("Found archive for XboxOne, applying to $dir...\n");
            UCopy($file, $dir);
        }
        elsif ($platformFilename =~ /android/)
        {
            $platformFilename =~ s/-android//;
            my $dir = "$root/PlatformDependent/AndroidPlayer/$destSubDir/$platformFilename.$platformSuffix";
            print("Found archive for Android, applying to $dir...\n");
            UCopy($file, $dir);
        }
        elsif ($platformFilename =~ /iOS/)
        {
            $platformFilename =~ s/-iOS//;
            my $dir = "$root/PlatformDependent/iPhonePlayer/$destSubDir/$platformFilename.$platformSuffix";
            print("Found archive for iOS, applying to $dir...\n");
            UCopy($file, $dir);
        }
        elsif ($platformFilename =~ /switch/)
        {
            $platformFilename =~ s/-switch//;
            my $dir = "$root/PlatformDependent/Switch/$destSubDir/$platformFilename.$platformSuffix";
            print("Found archive for Switch, applying to $dir...\n");
            UCopy($file, $dir);
        }
        elsif ($platformFilename =~ /webgl/)
        {
            $platformFilename =~ s/-webgl//;
            my $dir = "$root/PlatformDependent/WebGL/$destSubDir/$platformFilename.$platformSuffix";
            print("Found archive for WebGL, applying to $dir...\n");
            UCopy($file, $dir);
        }
        elsif ($platformFilename =~ /metro/)
        {
            $platformFilename =~ s/-metro//;
            my $dir = "$root/PlatformDependent/MetroPlayer/$destSubDir/$platformFilename.$platformSuffix";
            print("Found archive for Metro, applying to $dir...\n");
            UCopy($file, $dir);
        }
        elsif ($platformFilename =~ /winrt/)
        {
            $platformFilename =~ s/-winrt//;
            my $dir = "$root/PlatformDependent/WinRT/$destSubDir/$platformFilename.$platformSuffix";
            print("Found archive for WinRT, applying to $dir...\n");
            UCopy($file, $dir);
        }
        elsif ($platformFilename =~ /appletv/)
        {
            $platformFilename =~ s/-appletv//;
            my $dir = "$root/PlatformDependent/AppleTV/$destSubDir/$platformFilename.$platformSuffix";
            print("Found archive for AppleTV, applying to $dir...\n");
            UCopy($file, $dir);
        }
        elsif ($platformFilename =~ /non-secret/)
        {
            $platformFilename =~ s/-non-secret//;
            my $dir = "$root/$destSubDir/$platformFilename.$platformSuffix";
            print("Found archive for non-secret platforms, applying to $dir...\n");
            UCopy($file, $dir);
        }
        else
        {
            warn("Found archive for unknown/unsupported platform: $file\n");
            warn("This archive will not be applied automatically.\n");
            $unsupported = 1;
        }
    }

    # cleanup extracted temp files
    rmtree($targetdir);

    if ($unsupported)
    {
        print("\n==================================================\n");
        print("Not all archives were applied successfully -- please resolve the issue manually.\n");
        print("Then, check `hg status` for expected output and then commit the result with `hg commit`.\n");
        print("(It is up to you to verify that the resulting commit works on all platforms).\n");
        print("==================================================\n");
    }
    else
    {
        print("\n==================================================\n");
        print("All archives were applied successfully.\n");
        print("Check `hg status` for expected output and then commit the result with `hg commit`.\n");
        print("Note: In some cases the builds.zip(s) might need to be moved by hand to the proper location!.\n");
        print("(It is up to you to verify that the resulting commit works on all platforms).\n");
        print("==================================================\n");
    }
}

1;
