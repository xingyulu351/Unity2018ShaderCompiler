package Hoarder::Collectors::Tags;

use warnings;
use strict;

use Getopt::Long qw(GetOptionsFromArray :config pass_through);

sub new
{
    my $instance = { tags => [] };
    my $this = bless($instance, __PACKAGE__);

    $this->_collect();
    return $this;
}

sub name
{
    return 'tags';
}

sub data
{
    my ($this) = @_;
    return $this->{tags};
}

sub _collect
{
    my ($this) = @_;
    my @args = @ARGV;
    GetOptionsFromArray(\@args, 'tag:s@' => $this->{tags});
}

1;
