package TargetAttributesResolver;

use warnings;
use strict;

use Carp qw (croak);

use Dirs;
use lib Dirs::external_root();

use XML::Simple;

use List::MoreUtils qw (uniq);
use IntegrationTestTarget;
use StringUtils qw (trim);

sub new
{
    my ($pkg, $xml) = @_;
    my $xmlParser = new XML::Simple();
    my $tartgets = $xmlParser->XMLin($xml);
    my $instance = {
        src_targets => $tartgets->{Target},
        targets => _createTargetMap($tartgets->{Target}),
    };
    return bless($instance, __PACKAGE__);
}

sub getAllTargetNames
{
    my ($this) = @_;
    my @result;
    foreach my $key (keys(%{ $this->{targets} }))
    {
        push(@result, $key);
    }
    return @result;
}

sub resolve
{
    my ($this, $name) = @_;
    if (not defined($name))
    {
        return undef;
    }

    return $this->{targets}->{$name};
}

sub resolveDependencies
{
    my ($target) = @_;

    my (@includes, @excludes, @platforms);

    push(@includes, $target->getIncludes());
    push(@excludes, $target->getExcludes());
    push(@platforms, $target->getRuntimePlatforms());

    foreach my $dt ($target->getDependsOnTargets())
    {
        my ($dep_includes, $dep_excludes, $platforms) = resolveDependencies($dt);
        push(@includes, @$dep_includes);
        push(@excludes, @$dep_excludes);
        push(@platforms, @$platforms);
    }

    @includes = uniq(sort (@includes));
    @excludes = uniq(sort (@excludes));
    @platforms = uniq(sort (@platforms));
    return (\@includes, \@excludes, \@platforms);
}

sub _createTargetMap
{
    my @targets = _toArray(shift());

    my $result = {};

    # first pass - create all targets in a hashmap
    foreach my $t (@targets)
    {
        my $targetName = $t->{'Name'};
        if (not defined($targetName))
        {
            next;
        }
        $result->{$targetName} = _createTarget($t);
    }

    #second pass  - add dependant targets based on DependsOnTargets property
    foreach my $t (@targets)
    {
        my $dependsOnTargets = $t->{'DependsOnTargets'};
        if (not defined($dependsOnTargets))
        {
            next;
        }
        my $targetName = $t->{'Name'};
        my $parent = $result->{$targetName};
        my @dependsOnTargets = _getDependsOnTargets($dependsOnTargets, $result);
        $parent->addDependsOnTargets(@dependsOnTargets);
    }

    # third 3 resolve CallTargets
    foreach my $t (@targets)
    {
        my $callTarget = $t->{'CallTarget'};
        if (not defined $callTarget)
        {
            next;
        }
        my $targetName = $t->{'Name'};
        my $parent = $result->{$targetName};
        foreach my $ct (_toArray($callTarget))
        {
            my @dependsOnTargets = _getDependsOnTargets($ct->{Targets}, $result);
            $parent->addDependsOnTargets(@dependsOnTargets);
        }
    }

    return $result;
}

sub _toArray
{
    my ($scalar) = @_;
    my (@result);
    if (not ref($scalar) eq 'ARRAY')
    {
        push(@result, $scalar);
    }
    else
    {
        push(@result, @$scalar);
    }
    return @result;
}

sub _getDependsOnTargets
{
    my ($str, $targetsMap) = @_;
    my @dependsOnTargetNames = split(';', $str);
    my (@targets);
    foreach my $name (@dependsOnTargetNames)
    {
        $name = trim($name);
        my $targetName = $targetsMap->{$name};
        if (not defined($targetName))
        {
            croak("Can not find target '$name'");
        }
        push(@targets, $targetName);
    }
    return @targets;
}

sub _createTarget
{
    my $targetNode = shift();
    my $targetName = $targetNode->{Name};
    my $result = new IntegrationTestTarget();
    $result->setName($targetName);

    if (not defined($targetNode->{MSBuild}))
    {
        return $result;
    }

    if (not _isTestMsBuildTarget($targetNode))
    {
        return $result;
    }
    my @properties = split(';', $targetNode->{MSBuild}->{Properties});
    foreach my $property (@properties)
    {
        _addTargetProperties($property, $result);
    }
    return $result;
}

sub _isTestMsBuildTarget
{
    my ($msb) = @_;
    if (defined($msb->{MSBuild}) and ref($msb->{MSBuild}) ne 'ARRAY' and $msb->{MSBuild}->{Targets} eq 'RunTest')
    {
        return 1;
    }
    return 0;
}

sub _addTargetProperties
{
    my ($property_string, $target) = @_;
    my ($category, $attributes) = split('=', $property_string);
    if (not $category or not $attributes)
    {
        return;
    }
    foreach (split(',', $attributes))
    {
        my $attr = trim($_);
        if ($category eq 'IncludeCategory')
        {
            $target->addInclude($attr);
        }
        if ($category eq 'ExcludeCategory')
        {
            $target->addExclude($attr);
        }

        if ($category eq 'RuntimePlatforms')
        {
            $target->addPlatform($attr);
        }
    }
}

1;
