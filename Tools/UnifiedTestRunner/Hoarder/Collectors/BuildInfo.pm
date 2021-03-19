package Hoarder::Collectors::BuildInfo;

use warnings;
use strict;

use Getopt::Long qw(GetOptionsFromArray :config pass_through);
use StringUtils qw (isNullOrEmpty);

sub new
{
    my $instance = {
        config => undef,
        config_id => undef,
        build_number => undef
    };
    my $this = bless($instance, __PACKAGE__);
    $this->_collect();
    return $this;
}

sub name
{
    return 'build-info';
}

sub data
{
    my ($this) = @_;
    if ($this->_isIncomplete())
    {
        return undef;
    }
    return { %{$this} };
}

sub _isIncomplete
{
    my ($this) = @_;

    if (isNullOrEmpty($this->{config}))
    {
        return 1;
    }

    if (isNullOrEmpty($this->{config_id}))
    {
        return 1;
    }

    if (isNullOrEmpty($this->{build_number}))
    {
        return 1;
    }

    return 0;
}

sub _collect
{
    my ($this) = @_;
    my @args = @ARGV;
    GetOptionsFromArray(
        \@args,
        'config:s' => \$this->{config},
        'config-id:s' => \$this->{config_id},
        'build-number:s' => \$this->{build_number}
    );
}

1;
