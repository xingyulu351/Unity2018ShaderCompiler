package ProgressTracker;

use warnings;
use strict;

use threads::shared;

use Carp qw (croak);

sub new
{
    my ($pkg, %params) = @_;

    my $instance = {
        steps => {},
        step_errors => {},
        steps_order => [],
        listener => $params{listener},
        tracked_operation => $params{tracked_operation},
        step_errors => {},
    };

    return bless($instance, $pkg);
}

sub getTrackedOperation
{
    my ($this) = @_;
    return $this->{tracked_operation};
}

sub addStep
{
    my ($this, $step, $expected) = @_;
    $this->{steps}->{$step} = { executed => 0, expected => $expected };
    push(@{ $this->{steps_order} }, $step);
}

sub addStepError
{
    my ($this, $step, $error) = @_;
    if (not exists($this->{step_errors}{$step}))
    {
        $this->{step_erros}{$step} = [];
    }
    push(@{ $this->{step_errors}{$step} }, $error);
    if ($this->{listener})
    {
        $this->{listener}->stepError($step, $this, $error);
    }
}

sub stepErrorsCount
{
    my ($this, $step) = @_;
    if (not exists($this->{step_errors}{$step}))
    {
        return 0;
    }
    my @stepErrors = @{ $this->{step_errors}{$step} };
    return scalar(@stepErrors);
}

sub getTotalSteps
{
    my ($this) = @_;
    my @keys = keys(%{ $this->{steps} });
    return scalar(@keys);
}

sub getStepOrder
{
    my ($this, $step) = @_;
    my @stepOrder = @{ $this->{steps_order} };
    for (my $i = 0; $i < scalar(@stepOrder); $i++)
    {
        return $i + 1 if $stepOrder[$i] eq $step;
    }
    croak('Invalid step name');
}

sub incProgress
{
    my ($this, $step, $value, $description) = @_;
    if (not exists($this->{steps}{$step}))
    {
        $this->addStep($step, undef);
    }

    $this->{steps}{$step}{executed} += $value;
    if ($this->{listener} and not($this->_stepStartedSent($step)))
    {
        $this->{listener}->stepStarted($step, $this);
        $this->{started_steps}{$step} = 1;
    }

    if ($this->{listener} and $value > 0)
    {
        $this->{listener}->progressChange($step, $this, $description);
    }
}

sub info
{
    my ($this, $message) = @_;
    if (not defined($this->{listener}))
    {
        return;
    }
    $this->{listener}->info($message, $this);
}

sub stepDone
{
    my ($this, $step) = @_;
    if (not defined($this->{listener}) or ($this->_stepDoneSent($step)))
    {
        return;
    }
    $this->{listener}->stepDone($step, $this);
    $this->{done_steps}{$step} = 1;
}

sub getStepProgress
{
    my ($this, $step) = @_;
    return ($this->{steps}{$step}{executed}, $this->{steps}{$step}{expected});
}

sub _stepStartedSent
{
    my ($this, $step) = @_;
    return exists($this->{started_steps}{$step});
}

sub _stepDoneSent
{
    my ($this, $step) = @_;
    return exists($this->{done_steps}{$step});
}

1;
