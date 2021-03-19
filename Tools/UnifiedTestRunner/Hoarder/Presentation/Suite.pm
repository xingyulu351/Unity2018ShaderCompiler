package Hoarder::Presentation::Suite;

use warnings;
use strict;

use Hoarder::Presentation::Test;

sub new
{
    my ($pkg, %params) = @_;
    my $instance = {
        name => $params{name},
        platform => $params{platform},
        tests => [],
        testresultsxml => $params{testresultsxml}
    };
    return bless($instance, $pkg);
}

sub setPlatform
{
    my ($this, $value) = @_;
    $this->{platform} = $value;
}

sub data
{
    my ($this) = @_;
    return { %{$this} };
}

sub addTestResult
{
    my ($this, $testResult) = @_;

    my $test = new Hoarder::Presentation::Test(
        name => $testResult->getTest(),
        fixture => $testResult->getFixture(),
        time => $testResult->getTime(),
        state => $testResult->getState(),
        message => $testResult->getMessage()
    );
    push(@{ $this->{tests} }, $test);
}

1;
