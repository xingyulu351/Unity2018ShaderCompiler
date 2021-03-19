package ProgressPresenters;

use warnings;
use strict;
use Dirs;
use lib Dirs::external_root();

use Module::Pluggable instantiate => 'new', search_path => 'ProgressPresenters', require => 1, sub_name => 'makeProgressPresenters';

my @progressPresenters = makeProgressPresenters();

sub getProgressPresenter
{
    my ($name) = @_;
    foreach my $p (@progressPresenters)
    {
        return $p if $p->getName() eq $name;
    }
    return undef;
}

1;
