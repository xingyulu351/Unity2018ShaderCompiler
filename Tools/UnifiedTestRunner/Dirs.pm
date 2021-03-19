package Dirs;

use warnings;
use strict;

use File::Spec;
use Cwd qw (realpath);
use File::Basename qw (dirname);
use Cwd qw (abs_path getcwd);
use Carp qw (croak);
use File::Spec::Functions qw (canonpath);
use File::Find;
use File::Spec::Functions qw (canonpath);

my $root = undef;

sub getRoot
{
    if (defined($root))
    {
        return $root;
    }

    $root = canonpath(realpath(File::Spec->rel2abs(getcwd())));
    while (not isRepositoryRoot($root))
    {
        $root = canonpath(realpath(File::Spec->rel2abs($root . '/..')));
        if (isDriveRoot($root))
        {
            croak("\nRepository root not found\n");
        }
    }
    return $root;
}

sub external_root
{
    return canonpath(getRoot() . '/External/Perl/UTR');
}

sub UTR_root
{
    return realpath(dirname(__FILE__));
}

sub isDriveRoot
{
    my $path = shift();
    $path =~ s/\\/\//g;
    unless (substr($path, -1) eq "/")
    {
        $path .= "/";
    }

    my $slashesCount = $path =~ tr/\///;
    return ($slashesCount eq 1);
}

sub getFilesByExtentsionRecursive
{
    my ($dirname, @patterns) = @_;
    if (not -e $dirname)
    {
        croak("$dirname does not exist");
    }

    my @files;
    find(
        {
            wanted => sub
            {
                my $f = $File::Find::name;
                my $d = $File::Find::dir;
                if (_matchExtensions($f, @patterns)) { push(@files, canonpath($f)); }
            },
            no_chdir => 0,
            bydepth => 1
        },
        $dirname
    );

    return @files;
}

sub _matchExtensions
{
    my ($name, @patterns) = @_;

    for my $pattern (@patterns)
    {
        $pattern = quotemeta($pattern);
        if ($name =~ qr/$pattern$/i and not -d $name)
        {
            return 1;
        }
    }

    return 0;
}

sub isRepositoryRoot
{
    my $dir = shift();
    return (-e $dir . '/build.pl' and -d $dir . '/Tools');
}

1;
