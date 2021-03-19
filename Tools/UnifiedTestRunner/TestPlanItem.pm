package TestPlanItem;

use warnings;
use strict;

sub new
{
    my $package = shift;
    my %params = @_;
    my $obj = {};
    bless($obj, $package);
    $obj->{name} = $params{'name'};
    $obj->{testCaseCount} = $params{'testCaseCount'};
    return $obj;
}

sub getName
{
    my $this = shift;
    return $this->{name};
}

sub getTestCaseCount
{
    my ($this) = @_;
    return $this->{testCaseCount};
}

1;
