package ArtifactsMapEntry;

use warnings;
use strict;

use File::Spec::Functions qw (canonpath);

sub new
{
    my ($pkg, $name) = @_;
    my $instance = {
        name => $name,
        artifacts => []
    };
    return bless($instance, $pkg);
}

sub getName
{
    my ($this) = @_;
    return $this->{name};
}

sub addArtifact
{
    my ($this, $fileName) = @_;
    push(@{ $this->{artifacts} }, canonpath($fileName));
}

sub getArtifacts
{
    my ($this) = @_;
    return @{ $this->{artifacts} };
}

1;
