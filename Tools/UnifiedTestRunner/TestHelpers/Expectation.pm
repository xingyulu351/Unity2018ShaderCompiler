package TestHelpers::Expectation;
use Test::More;

use warnings;
use strict;

sub new
{
    my ($pkg, %params) = @_;
    my $instance = {
        params => $params{params},
        result => $params{result}
    };
    return bless($instance, $pkg);
}

sub match
{
    my ($this, %args) = @_;
    return eq_hash($this->{params}, \%args);
}

sub result
{
    my ($this) = @_;
    return $this->{result};
}

1;
