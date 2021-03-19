package PerformanceDataPostProcessor;
use warnings;
use strict;

sub embedPerformanceData
{
    my ($data, $suiteResult) = @_;
    my @testResults = $suiteResult->getAllResults();
    foreach my $tr (@testResults)
    {
        my $testName = $tr->getTest();
        my $performanceTestData = getTestData($data, $testName);
        $tr->addData('performanceTestResults', $performanceTestData);
    }
}

sub getTestData
{
    my ($suiteData, $testName) = @_;
    foreach my $tr (@$suiteData)
    {
        if ($tr->{testName} eq $testName)
        {
            return $tr;
        }
    }
    return undef;
}

1;
