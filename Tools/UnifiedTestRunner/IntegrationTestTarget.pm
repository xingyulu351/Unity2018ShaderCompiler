package IntegrationTestTarget;

use warnings;
use strict;

sub new
{
    my $instance = {
        name => '',
        includes => [],
        excludes => [],
        dependsOnTargets => [],
        platforms => [],
    };
    return bless($instance, __PACKAGE__);
}

sub setResolvedValues
{
    my ($this, $includes, $excludes, $platforms) = @_;
    @{ $this->{includes} } = @$includes;
    @{ $this->{excludes} } = @$excludes;
    @{ $this->{platforms} } = @$platforms;
}

sub setName
{
    my ($this, $value) = @_;
    $this->{name} = $value;
}

sub getName
{
    my ($this) = @_;
    return $this->{name};
}

sub addInclude
{
    my ($this, $value) = @_;
    push(@{ $this->{includes} }, $value);
}

sub addExclude
{
    my ($this, $value) = @_;
    push(@{ $this->{excludes} }, $value);
}

sub addDependsOnTargets
{
    my ($this, @targets) = @_;
    push(@{ $this->{dependsOnTargets} }, @targets);
}

sub getDependsOnTargets
{
    my ($this) = @_;
    return @{ $this->{dependsOnTargets} };
}

sub getIncludes
{
    my ($this) = @_;
    return @{ $this->{includes} };
}

sub getExcludes
{
    my ($this) = @_;
    return @{ $this->{excludes} };
}

sub addPlatform
{
    my ($this, $value) = @_;
    push(@{ $this->{platforms} }, $value);
}

sub getRuntimePlatforms
{
    my ($this) = @_;
    return @{ $this->{platforms} };
}

1;
