package ArtifactsMap;

use warnings;
use strict;

sub new
{
    my $instance = {
        tests => [],
        artifacts => []
    };
    return bless($instance, __PACKAGE__);
}

sub getTests
{
    my ($this) = @_;
    return @{ $this->{tests} };
}

sub addTest
{
    my ($this, $test) = @_;
    push(@{ $this->{tests} }, $test);
    $this->{ $test->getName() } = $test;
}

sub addArtifact
{
    my ($this, $artifact) = @_;
    push(@{ $this->{artifacts} }, $artifact);
}

sub getArtifacts
{
    my ($this) = @_;
    return @{ $this->{artifacts} };
}

sub getArtifactsForTest
{
    my ($this, $test) = @_;
    if ($this->{$test})
    {
        return $this->{$test}->getArtifacts();
    }
    return ();
}

1;
