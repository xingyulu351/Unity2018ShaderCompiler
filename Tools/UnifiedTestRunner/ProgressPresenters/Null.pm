package ProgressPresenters::Null;

use warnings;
use strict;

sub new
{
    return bless({}, __PACKAGE__);
}

sub getName
{
    my ($this) = @_;
    return 'null';
}

sub stepStarted
{
}

sub progressChange
{
}

sub stepDone
{
}

sub info
{
}

sub stepError
{
}

sub setPrinter
{
}

1;
