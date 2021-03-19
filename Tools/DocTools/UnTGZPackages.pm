use strict;
use warnings;
use File::Basename qw(dirname);
use File::Find;

my $root = File::Spec->rel2abs(dirname($0) . "/../..");

sub UnTGZPackages
{
    my $SevenZip;
    if ($^O eq 'MSWin32')
    {
        $SevenZip = "$root/External/7z/win32/7za.exe";
    }
    elsif ($^O eq "darwin")
    {
        $SevenZip = "$root/External/7z/osx/7za";
    }
    elsif ($^O eq 'linux')
    {
        $SevenZip = "$root/External/7z/linux64/7za";
    }
    else
    {
        die("Unsupported platform: $^O\n");
    }

    my @files;

    my $wanted = sub
    {
        my $filename = $File::Find::name;
        $filename =~ tr/\\/\//;
        if ($filename =~ /Resources\/PackageManager\/Editor\/.*\.tgz$/)
        {
            push @files, $filename;
        }
    };

    find({ wanted => $wanted }, "$root/build");

    my $packagetemp;
    foreach my $filename (@files)
    {
        my $dir = dirname($filename);
        my $tarname = $filename;
        $tarname =~ s/\.tgz$/\.tar/;
        my $filebasename = basename($filename);
        my $outputdir = "$root/build/temp/docs_untgz/$filebasename";
        if (!$packagetemp)
        {
            $packagetemp = File::Spec->rel2abs(dirname($outputdir));
        }

        $outputdir =~ s/\.tgz$//;

        mkpath($outputdir);
        my @cmdUnGzip = ($SevenZip, 'x', $filename, "-o$dir");
        map { print "$_ " } (@cmdUnGzip, "\n");
        system(@cmdUnGzip) and die("Failed to ungzip $filename\n");

        my @cmdUnTar = ($SevenZip, 'x', '-y', '-ttar', $tarname, "-o$outputdir");
        map { print "$_ " } (@cmdUnTar, "\n");
        system(@cmdUnTar) and die("Failed to untar $tarname\n");

        unlink($tarname);
    }

    return $packagetemp;
}
