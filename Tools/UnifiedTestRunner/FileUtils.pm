package FileUtils;

use warnings;
use strict;

use File::Basename;
use File::Spec::Functions qw (canonpath);
use Dirs;
use Carp qw (croak);
use English qw (-no_match_vars);
use Dirs;
use lib Dirs::external_root();
use Try::Tiny;
use JSON;

use Exporter 'import';
our @EXPORT_OK = qw (getDirName
    removeSubPath
    openForReadOrCroak
    openForWriteOrCroak
    closeOrCroak
    scanLines
    readAllLines
    writeAllLines
    readJsonObj
    touch
);

sub getDirName
{
    my ($fileName) = @_;
    my (undef, $path, undef) = fileparse($fileName);
    return canonpath($path);
}

sub removeSubPath
{
    my ($srcPath, $srcSubPath) = @_;
    my $path = canonpath($srcPath);
    if (not defined($srcSubPath))
    {
        return $path;
    }

    my $subpath = canonpath($srcSubPath);
    if (index($path, $subpath) == 0)
    {
        return substr($path, length($subpath) + 1, length($path) - length($subpath) - 1);
    }
    return $path;
}

sub readAllLines
{
    my ($file) = @_;
    my $openres = open(my $fh, '<', $file);
    if (not defined($openres))
    {
        croak("Can not read from the file $file. $OS_ERROR");
    }
    my @result = _readLines($fh);
    my $closeRes = close($fh);
    if (not $closeRes)
    {
        croak("Can not close the file $file. $OS_ERROR");
    }
    return @result;
}

sub _readLines
{
    my ($fh) = @_;
    my @result;
    while (<$fh>)
    {
        my $line = $_;
        chomp($line);
        push(@result, $line);
    }
    return @result;
}

sub writeAllLines
{
    my ($file, @lines) = @_;
    my $openres = open(my $fh, '>:encoding(UTF-8)', $file);
    if (not defined($openres))
    {
        croak("Can not write to the file $file. $OS_ERROR");
    }
    foreach my $line (@lines)
    {
        print($fh "$line\n");
    }
    my $closeRes = close($fh);
    if (not $closeRes)
    {
        croak("Can not close the file $file. $OS_ERROR");
    }
}

sub openForReadOrCroak
{
    my ($filename) = @_;
    my $openRes = open(my $fh, '<', $filename);
    if (not defined($openRes))
    {
        croak("Can not read from the file $filename. $OS_ERROR");
    }
    return $fh;
}

sub openForWriteOrCroak
{
    my ($filename) = @_;
    my $openRes = open(my $fh, '>', $filename);
    if (not defined($openRes))
    {
        croak("Can not write to the file $filename. $OS_ERROR");
    }
    return $fh;
}

sub closeOrCroak
{
    my ($fh, $filename) = @_;
    my $closeRes = close($fh);
    if (not $closeRes)
    {
        croak("Can not close the file $filename. $OS_ERROR\n");
    }
}

sub fileExists
{
    my ($fileName) = @_;
    return -e $fileName;
}

sub scanLines
{
    my %params = @_;
    my $fh = openForReadOrCroak($params{file});
    my $callback = $params{func};
    my @errors;
    try
    {
        while (<$fh>)
        {
            my $line = $_;
            chomp($line);
            my $res = &{$callback}($line);
            if ($res == 0)
            {
                last;
            }
        }
    }
    finally
    {
        closeOrCroak($fh, $params{file});
        @errors = @_;
    };

    if (@errors)
    {
        croak(@errors);
    }
}

sub readJsonObj
{
    my ($fileName) = @_;
    my $text = join(q (), readAllLines($fileName));
    return jsonToObj($text);
}

sub touch
{
    my ($fileName) = @_;
    my $fh = openForWriteOrCroak($fileName);
    close($fh);
}

1;
