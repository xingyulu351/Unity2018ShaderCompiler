package TestResult;
use warnings;
use strict;

use Dirs;
use lib Dirs::external_root();
use Switch;

use Carp qw (croak);

# corresponds to nunit 2.6.x constants
use constant {
    INCONCLUSIVE => 0,
    NOTRUNNABLE => 1,
    SKIPPED => 2,
    IGNORED => 3,
    SUCCESS => 4,
    FAILURE => 5,
    ERROR => 6
};

sub new
{
    my $instance = {
        suite => undef,
        fixture => undef,
        test => undef,
        reason => undef,
        state => INCONCLUSIVE,
        time => 0,
        message => undef,
        platform => undef,
        artifacts => [],
        stackTrace => undef,
        data => {}
    };
    return bless($instance, __PACKAGE__);
}

sub setParent
{
    my ($this, $value) = @_;
    $this->{parent} = $value;
}

sub getSuite
{
    my ($this) = @_;
    return $this->{parent}->getName();
}

sub setFixture
{
    my ($this, $value) = @_;
    $this->{fixture} = $value;
}

sub getFixture
{
    my ($this) = @_;
    return $this->{fixture};
}

sub setTest
{
    my ($this, $value) = @_;
    $this->{test} = $value;
}

sub getTest
{
    my ($this) = @_;
    return $this->{test};
}

sub setState
{
    my ($this, $value) = @_;
    $this->{state} = $value;
}

sub getState
{
    my ($this) = @_;
    return $this->{state};
}

sub setTime
{
    my ($this, $value) = @_;
    $this->{time} = $value;
}

sub getTime
{
    my ($this) = @_;
    return $this->{time};
}

sub getPlatform
{
    my ($this) = @_;
    return $this->{parent}->{platform};
}

sub isSucceeded
{
    my ($this) = @_;
    return $this->_stateIs(SUCCESS);
}

sub isFailed
{
    my ($this) = @_;
    return $this->_stateIs(FAILURE);
}

sub isIgnored
{
    my ($this) = @_;
    return $this->_stateIs(IGNORED);
}

sub isSkipped
{
    my ($this) = @_;
    return $this->_stateIs(SKIPPED);
}

sub isInconclusive
{
    my ($this) = @_;
    return $this->_stateIs(INCONCLUSIVE);
}

sub isError
{
    my ($this) = @_;
    return $this->_stateIs(ERROR);
}

sub isNotRunnable
{
    my ($this) = @_;
    return $this->_stateIs(NOTRUNNABLE);
}

sub setMessage
{
    my ($this, $value) = @_;
    $this->{message} = $value;
}

sub getMessage
{
    my ($this) = @_;
    return $this->{message};
}

sub setArtifacts
{
    my ($this, @artifacts) = @_;
    $this->{artifacts} = \@artifacts;
}

sub getArtifacts
{
    my ($this) = @_;
    return @{ $this->{artifacts} };
}

sub getStackTrace
{
    my ($this) = @_;
    return $this->{stackTrace};
}

sub setStackTrace
{
    my ($this, $value) = @_;
    $this->{stackTrace} = $value;
}

sub addData
{
    my ($this, $keyName, $value) = @_;
    $this->{data}->{$keyName} = $value;
}

sub getData
{
    my ($this) = @_;
    return $this->{data};
}

sub getReason
{
    my ($this) = @_;
    return $this->{reason};
}

sub setReason
{
    my ($this, $value) = @_;
    $this->{reason} = $value;
}

sub getStateAsString
{
    my ($this) = @_;
    my $state = stateToString($this->getState());
    if ($state eq "Unknown")
    {
        croak("Unsupported test result value $state");
    }
    return $state;
}

sub stateToString
{
    my ($state) = @_;
    switch ($state)
    {
        case TestResult::INCONCLUSIVE { return 'INCONCLUSIVE'; }
        case TestResult::NOTRUNNABLE { return 'NOTRUNNABLE'; }
        case TestResult::SKIPPED { return 'SKIPPED' }
        case TestResult::IGNORED { return 'IGNORED'; }
        case TestResult::SUCCESS { return 'SUCCESS'; }
        case TestResult::FAILURE { return 'FAILURE'; }
        case TestResult::ERROR { return 'ERROR'; }
        else { return "Unknown"; }
    }
}

sub _stateIs
{
    my ($this, $state) = @_;
    my $s = $this->getState();
    if ($s eq $state)
    {
        return 1;
    }
    return 0;
}

1;
