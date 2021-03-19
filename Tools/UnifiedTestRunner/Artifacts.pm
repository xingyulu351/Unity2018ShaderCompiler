package Artifacts;

use ArtifactsNameBuilder;

use warnings;
use strict;

sub new
{
    my ($package) = shift;
    my $instance = { artifacts => {} };
    return bless($instance, $package);
}

sub register
{
    my ($this, $suite, $artifact) = @_;
    if (not exists $this->{artifacts}{$suite})
    {
        $this->{artifacts}{$suite} = ();
    }

    push(@{ $this->{artifacts}{$suite} }, $artifact);
}

sub get
{
    my ($this, $suite) = @_;
    if (not exists($this->{artifacts}{$suite}))
    {
        return ();
    }
    return @{ $this->{artifacts}{$suite} };
}

1;
