package ArtifactsNameBuilder;

use warnings;
use strict;

use File::Spec::Functions qw (canonpath);
use Carp qw (croak);

#TODO: rename to getArtifactFolderLeafName
#TODO: make functions exportable
sub getSuiteRunArtifactsFolderName
{
    my (%params) = @_;
    my $suite = $params{suite};
    if (not defined($suite))
    {
        croak('suite must be defined when request for artifact name');
    }

    my $platform_or_target = $params{platform_or_target};
    if (not defined($platform_or_target))
    {
        croak('platform must be defined when request for artifact name');
    }

    return canonpath("$platform_or_target/$suite");
}

sub getFolderNameForArtifactType
{
    my (%params) = @_;
    my $type = $params{type};
    if (not defined($type))
    {
        croak('type must be defined when request for artifact name');
    }
    return $type;
}

sub getName
{
    my %params = @_;

    my $type = $params{type};
    if (not defined($type))
    {
        croak('type must be defined when request for artifact name');
    }

    my $result = "$type";
    my $extension = $params{extension};
    if ($extension)
    {
        $result .= ".$extension";
    }

    return $result;
}

1;
