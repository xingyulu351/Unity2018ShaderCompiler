package Report::Presentation::Test;

use warnings;
use strict;

sub new
{
    my ($pkg, $testResult) = @_;
    my @artifacts = $testResult->getArtifacts();
    my $instance = {
        name => $testResult->getTest(),
        fixture => $testResult->getFixture(),
        time => $testResult->getTime(),
        state => $testResult->getState(),
        message => $testResult->getMessage(),
        artifacts => \@artifacts,
        reason => $testResult->getReason(),
        stackTrace => $testResult->getStackTrace(),
        data => $testResult->getData()
    };
    return bless($instance, $pkg);
}

sub data
{
    my ($this) = @_;
    return { %{$this} };
}

1;
