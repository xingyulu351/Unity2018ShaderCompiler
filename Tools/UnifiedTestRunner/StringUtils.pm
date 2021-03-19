package StringUtils;

use warnings;
use strict;

use Exporter 'import';

our @EXPORT_OK = qw (isNullOrEmpty trimStart trimEnd trim);

sub isNullOrEmpty
{
    my ($string) = @_;
    if (not defined($string) or $string eq '')
    {
        return 1;
    }
    return 0;
}

sub trimStart
{
    my ($string) = @_;
    if (isNullOrEmpty($string))
    {
        return $string;
    }
    $string =~ s/^\s+//;
    return $string;
}

sub trimEnd
{
    my ($string) = @_;
    if (isNullOrEmpty($string))
    {
        return $string;
    }
    $string =~ s/\s+$//;
    return $string;
}

sub trim
{
    my ($string) = @_;
    $string = trimStart($string);
    $string = trimEnd($string);
    return $string;
}

1;
