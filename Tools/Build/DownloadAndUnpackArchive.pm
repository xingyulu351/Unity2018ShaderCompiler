#!/usr/bin/perl

use strict;
use warnings;
use Getopt::Long;
use Carp qw(croak carp);
use File::Path qw(mkpath rmtree);
use File::Spec::Functions;
use File::Copy;
use File::Basename;

our @EXPORT_OK = qw(DownloadAndUnpackArchive);

sub DownloadAndUnpackArchive
{
    my ($url, $output, $compressed_file, $ndk) = @_;

    my ($HOST_ENV, $TMP, $HOME, $WINZIP);

    if (lc $^O eq 'darwin')
    {
        $HOST_ENV = "macosx";
        $TMP = $ENV{"TMPDIR"};
        $HOME = $ENV{"HOME"};
    }
    elsif (lc $^O eq 'linux')
    {
        $HOST_ENV = "linux";
        $TMP = "/tmp";
        $HOME = $ENV{"HOME"};
    }
    elsif (lc $^O eq 'mswin32')
    {
        $HOST_ENV = "windows";
        $TMP = $ENV{"TMP"};
        $HOME = $ENV{"USERPROFILE"};
        if (-e "External/7z/win32/7za.exe")
        {
            $WINZIP = "External/7z/win32/7za.exe";
        }
    }
    elsif (lc $^O eq 'cygwin')
    {
        $HOST_ENV = "windows";
        $TMP = $ENV{"TMP"};
        $HOME = $ENV{"HOME"};
    }
    else
    {
        die "UNKNOWN " . $^O;
    }

    my ($base, $base_url) = fileparse($url, qr/\.[^.]*/);
    my ($dest_name, $dest_path) = fileparse($output);
    my ($branch, $notUsed2, $suffix) = fileparse($compressed_file, qr/\.\D.*/);
    my $temporary_download_path = catfile($TMP, $base);
    my $temporary_download_file = catfile($temporary_download_path, $compressed_file);
    my $temporary_unpack_path = catfile($TMP, $base . "_unpack");

    print "\t\tURL: " . $url . "\n";
    print "\t\tBranch: " . $branch . "\n";
    print "\t\tOutput: " . $output . "\n";
    print "\t\tBase: " . $base . "\n";
    print "\t\tURL base: " . $base_url . "\n";
    print "\t\tSuffix: " . $suffix . "\n";
    print "\t\tTmp DL: " . $temporary_download_path . "\n";
    print "\t\tTmp DL File: " . $temporary_download_file . "\n";
    print "\t\tTmp unpack: " . $temporary_unpack_path . "\n";
    print "\t\tDest path: " . $dest_path . "\n";
    print "\t\tDest name: " . $dest_name . "\n";
    print "\t\tFile: " . $compressed_file . "\n";

    # remove old output
    rmtree($output);
    mkpath($dest_path);

    # create temporary locations
    rmtree($temporary_download_path);
    rmtree($temporary_unpack_path);
    mkpath($temporary_download_path);
    mkpath($temporary_unpack_path);
    print "\t\t Cloning Mercurial Repository.\n";
    system("hg clone -b $branch $url $temporary_download_path") == 0 or die("Failed to clone branch -- sdk doesn't exist or improper mercurial privileges");

    if ($WINZIP)
    {
        system($WINZIP, "x", $temporary_download_file, "-o" . $temporary_unpack_path);
    }
    else
    {
        if (lc $suffix =~ m/\.zip/)
        {
            system("unzip", "-q", $temporary_download_file, "-d", $temporary_unpack_path);
        }
        elsif (lc $suffix =~ m/\.tar\.bz2/)
        {
            system("tar", "-xf", $temporary_download_file, "-C", $temporary_unpack_path);
        }
        else
        {
            die "Unknown file extension '" . $suffix . "'\n";
        }
    }

    move($temporary_unpack_path, $output);

    # clean up
    rmtree($temporary_download_path);
    rmtree($temporary_unpack_path);

    # Write out file to tag this directory as a paticular NDK version.
    my $release_file = catfile($output, "RELEASE.TXT");
    system("echo $ndk > $release_file");
}
