package Hoarder::Collectors;

use warnings;
use strict;

use Dirs;
use Carp qw (croak);
use File::Spec::Functions qw (catfile catdir);
use SystemCall;
use Revision;

use Module::Pluggable
    instantiate => 'new',
    search_path => 'Hoarder::Collectors',
    require => 1,
    sub_name => 'makeCollectors',
    on_instantiate_error => sub
{
    my ($plugin, $error) = @_;
    croak("Failed to load $plugin: $error\n");
};

my $revision = Revision::get();
my @collectors = makeCollectors(optional => ($revision));

sub getAllCollectors
{
    return @collectors;
}

1;
