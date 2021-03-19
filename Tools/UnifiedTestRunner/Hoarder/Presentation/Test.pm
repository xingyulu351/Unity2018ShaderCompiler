package Hoarder::Presentation::Test;

use warnings;
use strict;

sub new
{
    my ($pkg, %params) = @_;
    my $instance = {
        name => $params{name},
        fixture => $params{fixture},
        state => $params{state},
        time => $params{time},
        message => $params{message},
        errors => $params{errors}
    };

    return bless($instance, $pkg);
}

sub data
{
    my ($this) = @_;
    return { %{$this} };
}

1;
