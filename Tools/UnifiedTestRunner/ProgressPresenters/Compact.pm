package ProgressPresenters::Compact;

use warnings;
use strict;

use Log::Log4perl qw(:easy);
use Data::Dumper;

sub new
{
    my $instance = {};
    return bless($instance, __PACKAGE__);
}

my $descriptionWidth = 120;
my $isWaitingForTestConclusion = 0;

sub getName
{
    return "compact";
}

sub setDescriptionWidth
{
    my ($this, $width) = @_;
    $descriptionWidth = $width;
}

sub stepStarted
{
    my ($this, %args) = @_;

    if ($isWaitingForTestConclusion)
    {
        # We're being used by a runner that doesn't produce test-ended messages correctly
        INFO("\n");
    }

    my $countWidth = 3;
    my $progress = $args{progress};
    my $total = $progress->{total};
    if (not defined($total))
    {
        $total = '?';
    }
    else
    {
        $countWidth = int(POSIX::ceil(log($total) / log(10)));
        if ($countWidth < 1)
        {
            $countWidth = 1;
        }
    }
    my $current = $progress->{current};

    my $formatString = "[%" . $countWidth . "i/%s] %-" . $descriptionWidth . "s ";

    my $msg = sprintf($formatString, $current, $total, $args{progressDetails});
    INFO($msg);

    $isWaitingForTestConclusion = 1;
}

sub stepDone
{
    my ($this, %args) = @_;

    if (exists $args{state})
    {
        my $msg = sprintf("%10s, %4.2fms\n", TestResult::stateToString($args{state}), $args{duration});
        INFO($msg);
    }
    else
    {
        # not sure what to do here - just end the line
        INFO("\n");
    }

    $isWaitingForTestConclusion = 0;
}

sub stepError
{
    my ($this, %args) = @_;
    $this->stepDone(%args);
}

sub runFinished
{
    my ($this, %args) = @_;
    if ($isWaitingForTestConclusion)
    {
        INFO("\n");
    }
    $isWaitingForTestConclusion = 0;
}

1;
