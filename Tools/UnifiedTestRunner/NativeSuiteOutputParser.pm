package NativeSuiteOutputParser;

use warnings;
use strict;

use Carp qw (croak);

use TestResult;

sub new
{
    my $class = shift;
    my @results = ();
    my $self = {
        suite => shift,
        parsing_message => 0,
        met_summary => 0,
        success_count => 0,
        failure_count => 0,
        disable_count => 0,
        expecting_state => 0,
        previous_line => 0,
        results => \@results
    };

    return bless($self, $class);
}

sub setSuite
{
    my ($this, $value) = @_;
    return $this->{suite} = $value;
}

sub getSuite
{
    my ($this) = @_;
    return $this->{suite};
}

sub isParsingMessage
{
    my ($this) = @_;
    return $this->{parsing_message};
}

sub isEof
{
    my ($this, $line) = @_;
    return 1 if $this->{met_summary};

    my ($testsCount, $fails) = ($line =~ m /^Ran\s+(\d+).*(\d+)\sfailures/);
    if (not $testsCount)
    {
        return 0;
    }

    $this->{met_summary} = 1;

    my $total_tests = $this->{success_count} + $this->{failure_count};
    if ($total_tests != $testsCount)
    {
        croak("Expected number of tests: " . $testsCount . " but was " . $total_tests);
    }

    return 1;
}

sub processLine
{
    my ($this, $line) = @_;
    chomp($line);
    if ($this->isEof($line))
    {
        return;
    }

    # note: test names are detected with "\S+", since they can contain parentheses
    if ($this->{expecting_state})
    {
        my ($state) = ($line =~ m /(\w+)/);
        if (isCorrectState($state))
        {
            my ($fixture, $test) = ($this->{previous_line} =~ m /\[(\w+)\]\s+(\S+)/);
            $line = "[$fixture] $test $line";
        }
        else
        {
            return;
        }
    }

    my ($fixture, $test) = ($line =~ m /\[(\w+)\]\s+(\S+)/);
    if (defined($fixture) and defined($test))
    {
        my ($state) = ($line =~ m /\[\w+\]\s+\S+\s+(\w+)/);
        if (not isCorrectState($state))
        {
            $this->{expecting_state} = 1;
            $this->{previous_line} = $line;
            return;
        }
        else
        {
            $this->{expecting_state} = 0;
            $this->{previous_line} = undef;
        }

        $this->parseTest($fixture, $test, $state, $line);
        return;
    }

    if ($this->isParsingMessage())
    {
        $this->parseMessage($line);
        return;
    }
}

sub parseTest
{
    my ($this, $fixture, $test, $state, $line) = @_;
    my $result = TestResult->new();
    if (isPassedTestState($state))
    {
        $result->setState(TestResult::SUCCESS);
        my ($time) = $line =~ m /\((\d+)/;
        $result->setTime($time);
        ++$this->{success_count};
    }
    elsif (isFailedTestState($state))
    {
        $result->setState(TestResult::FAILURE);
        $this->{parsing_message} = 1;
        ++$this->{failure_count};
    }
    elsif (isDisabledTestState($state))
    {
        $result->setState(TestResult::IGNORED);
        $this->{parsing_message} = 1;
        ++$this->{disable_count};
    }
    else
    {
        croak('Invalid state');
    }

    $result->setFixture($fixture);
    $result->setTest($test);
    push(@{ $this->{results} }, $result);
}

sub isCorrectState
{
    my ($state) = @_;
    if (not defined($state))
    {
        return 0;
    }
    return (isPassedTestState($state) or isFailedTestState($state) or isDisabledTestState($state));
}

sub isPassedTestState
{
    my ($state) = @_;
    return $state eq 'PASS';
}

sub isFailedTestState
{
    my ($state) = @_;
    return $state eq 'FAIL';
}

sub isDisabledTestState
{
    my ($state) = @_;
    return $state eq 'DISABLED';
}

sub parseMessage
{
    my ($this, $line) = @_;
    my $last_result = @{ $this->{results} }[-1];
    my $message = @{ $this->{results} }[-1]->getMessage();
    if ($message)
    {
        $message = join("\n", $message, $line);
    }
    else
    {
        $message = $line;
    }
    $last_result->setMessage($message);
}

sub getFailureCount
{
    my ($this) = @_;
    return $this->{failure_count};
}

sub getSuccessCount
{
    my ($this) = @_;
    return $this->{success_count};
}

sub getDisabledCount
{
    my ($this) = @_;
    return $this->{disable_count};
}

sub getResults
{
    my ($this) = @_;
    return @{ $this->{results} };
}

sub getResultsCount
{
    my ($this) = @_;
    return scalar(@{ $this->{results} });
}

1;
