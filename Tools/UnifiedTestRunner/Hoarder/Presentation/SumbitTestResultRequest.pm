package Hoarder::Presentation::SumbitTestResultRequest;

use warnings;
use strict;

my $PROTOCOL_VERSION = 1;

use Hoarder::Presentation::Suite;

sub new
{
    my $instance = {
        suites => [],
        collectors => {},
        displayOrder => []
    };
    return bless($instance, __PACKAGE__);
}

sub addCollectorData
{
    my ($this, $collectorName, $data) = @_;
    $this->{collectors}{$collectorName} = $data;
}

sub addSuiteResults
{
    my ($this, @suiteResults) = @_;
    foreach my $sr (@suiteResults)
    {
        my $suiteName = $sr->getName();
        my $testResultsXml = $sr->getTestResultXmlFilePath();

        my $suite = new Hoarder::Presentation::Suite(
            name => $suiteName,
            platform => $sr->getPlatform(),
            testresultsxml => $testResultsXml
        );

        push(@{ $this->{suites} }, $suite);
        if (not defined($testResultsXml))
        {
            foreach my $tr ($sr->getAllResults())
            {
                $suite->addTestResult($tr);
            }
        }
    }
}

sub setSmartSelectSessionId
{
    my ($this, $smartSelectSessionId) = @_;
    $this->{smartSelectSessionId} = $smartSelectSessionId;
}

sub data
{
    my ($this) = @_;
    my $data = {
        suites => [],
        smartSelectSessionId => $this->{smartSelectSessionId},
    };

    $data->{version} = $PROTOCOL_VERSION;
    foreach my $name (keys %{ $this->{collectors} })
    {
        $data->{$name} = $this->{collectors}{$name};
    }
    push(@{ $data->{suites} }, @{ $this->{suites} });
    return $data;
}

1;
