package OptionsMerger;

use warnings;
use strict;

use Dirs;
use lib Dirs::external_root();

use Array::Utils qw (intersect);

sub merge
{
    my ($set1, $set2) = @_;
    if (not defined($set1))
    {
        $set1 = [];
    }

    my @result = (@{$set1});
    foreach my $o (@{$set2})
    {
        my @names = $o->getNames();
        my $idx = _contains(\@result, \@names);
        if ($idx == -1)
        {
            push(@result, $o);
        }
        else
        {
            if ($result[$idx]->{isPrototype})
            {
                $result[$idx] = $o;
            }
        }
    }
    return \@result;
}

sub exclude
{
    my ($set1, $set2) = @_;
    my @result;
    foreach my $o (@{$set1})
    {
        my @names = $o->getNames();
        if (_contains($set2, \@names) == -1)
        {
            push(@result, $o);
        }
    }
    return \@result;
}

sub _contains
{
    my ($options, $names) = @_;

    my $idx = 0;
    foreach my $o (@{$options})
    {
        my @currentOptionNames = $o->getNames();
        if (intersect(@currentOptionNames, @{$names}))
        {
            return $idx;
        }
        ++$idx;
    }

    return -1;
}

1;
