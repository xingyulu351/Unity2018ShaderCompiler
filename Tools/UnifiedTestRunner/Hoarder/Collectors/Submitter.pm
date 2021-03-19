package Hoarder::Collectors::Submitter;

use warnings;
use strict;

use Getopt::Long qw (GetOptionsFromArray :config pass_through);

use StringUtils qw (isNullOrEmpty);

sub new
{
    my $instance = { submitter => _getSubmitter() };
    return bless($instance, __PACKAGE__);
}

sub name
{
    return 'submitter';
}

sub data
{
    my ($this) = @_;
    if ($this->_isIncomplete())
    {
        return undef;
    }

    return $this->{submitter};
}

sub _isIncomplete
{
    my ($this) = @_;

    if (isNullOrEmpty($this->{submitter}))
    {
        return 1;
    }

    return 0;
}

sub _getSubmitter
{
    my @args = @ARGV;
    my $submitter;
    GetOptionsFromArray(\@args, 'submitter:s' => \$submitter,);
    return $submitter;
}

1;
