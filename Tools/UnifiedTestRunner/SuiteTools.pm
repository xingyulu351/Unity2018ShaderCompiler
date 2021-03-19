package SuiteTools;

use warnings;
use strict;
use Dirs;
use lib Dirs::external_root();

use Module::Pluggable instantiate => 'new', search_path => 'SuiteTools', require => 1, sub_name => '_makeTools';

our @tools;

sub getTools
{
    if (not @tools)
    {
        @tools = _makeTools();
    }
    return @tools;
}

sub getTool
{
    my ($name) = @_;
    my @tools = getTools();
    foreach my $t (@tools)
    {
        return $t if $t->getName() eq $name;
    }
    return undef;
}

sub getOptions
{
    my @allOptions;
    my @tools = getTools();
    foreach my $tool (@tools)
    {
        my $toolOptions = $tool->getOptions();
        push(@allOptions, @{$toolOptions});
    }
    return @allOptions;
}

1;
