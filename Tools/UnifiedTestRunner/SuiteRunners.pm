package SuiteRunners;

use warnings;
use strict;

use Dirs;
use lib Dirs::external_root();
use Module::Pluggable instantiate => 'new', search_path => 'SuiteRunners', require => 1, sub_name => '_makeRunners';

our @runners;

sub getRunners
{
    if (not @runners)
    {
        @runners = _makeRunners();
    }
    return @runners;
}

sub getRunner
{
    my ($name) = @_;
    my @runners = getRunners();
    foreach my $r (@runners)
    {
        return $r if lc($r->getName()) eq lc($name);
    }
    return undef;
}

sub getOptions
{
    my @allOptions;
    my @runners = getRunners();
    foreach my $runner (@runners)
    {
        my $runnerOptions = $runner->getOptions();
        push(@allOptions, @{$runnerOptions});
    }
    return @allOptions;
}

1;
