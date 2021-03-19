package SummaryBuilder;

use warnings;
use strict;

use TestResult;

use Dirs;
use lib Dirs::external_root();
use Switch;

sub build
{
    my @suiteResults = @_;
    my %summary = createSummary(@suiteResults);
    return %summary;
}

sub createSummary
{
    my (@suiteResults) = @_;

    my @suiteSummaries;
    foreach my $s (@suiteResults)
    {
        my %ss = suiteSummary($s);
        push(@suiteSummaries, \%ss);
    }

    return merge(@suiteSummaries);
}

sub merge
{
    my (@suiteSummaries) = @_;
    my %summary = (
        testsCount => 0,
        successCount => 0,
        errorCount => 0,
        failedCount => 0,
        time => 0,
        ignoredCount => 0,
        skippedCount => 0,
        notRunCount => 0,
        inconclusiveCount => 0,
        success => 0,
        result => TestResult::INCONCLUSIVE,
        hasSuiteWithNoTests => 0,
    );

    if (scalar(@suiteSummaries) == 0)
    {
        return %summary;
    }

    my $atLeastOneSuiteIsForciblyFailed = 0;
    my $atLeastOneSuiteHasZeroTestRuns = 0;
    my $zeroTestsAreOk = 0;
    foreach my $ss (@suiteSummaries)
    {
        $summary{testsCount} += $ss->{testsCount};
        $summary{successCount} += $ss->{successCount};
        $summary{errorCount} += $ss->{errorCount};
        $summary{failedCount} += $ss->{failedCount};
        $summary{time} += $ss->{time};
        $summary{ignoredCount} += $ss->{ignoredCount};
        $summary{notRunCount} += $ss->{notRunCount};
        $summary{skippedCount} += $ss->{skippedCount};
        $summary{inconclusiveCount} += $ss->{inconclusiveCount};

        if ($ss->{zeroTestsAreOk})
        {
            $zeroTestsAreOk = 1;
        }
        if ($ss->{isForcibilyFailed})
        {
            $atLeastOneSuiteIsForciblyFailed = 1;
        }

        if ($ss->{notRunCount} == $ss->{testsCount})
        {
            $summary{hasSuiteWithNoTests} = 1;
            $atLeastOneSuiteHasZeroTestRuns = 1;
        }
    }

    my $runableTestsCount = $summary{testsCount} - $summary{notRunCount};

    if ($summary{successCount} > 0 && $summary{successCount} == $runableTestsCount)
    {
        $summary{result} = TestResult::SUCCESS;
    }

    if (   $summary{failedCount} > 0
        || $summary{errorCount} > 0
        || $summary{hasSuiteWithNoTests} == 1
        || $summary{inconclusiveCount} > 0)
    {
        $summary{result} = TestResult::FAILURE;
    }

    if ($summary{inconclusiveCount} == $runableTestsCount)
    {
        $summary{result} = TestResult::INCONCLUSIVE;
    }

    if ($atLeastOneSuiteHasZeroTestRuns && !$atLeastOneSuiteIsForciblyFailed && $runableTestsCount > 0 && $zeroTestsAreOk)
    {
        $summary{result} = TestResult::SUCCESS;
    }

    if ($atLeastOneSuiteIsForciblyFailed)
    {
        $summary{result} = TestResult::FAILURE;
    }

    if ($summary{result} == TestResult::SUCCESS)
    {
        $summary{success} = 1;
    }

    return %summary;
}

sub suiteSummary
{
    my ($suiteResult) = @_;

    my %summary = (
        name => $suiteResult->getName(),
        testsCount => 0,
        successCount => 0,
        errorCount => 0,
        failedCount => 0,
        time => 0,
        ignoredCount => 0,
        skippedCount => 0,
        notRunCount => 0,
        inconclusiveCount => 0,
        result => TestResult::FAILURE,
        success => 1,
        isForcibilyFailed => 0,
        zeroTestsAreOk => $suiteResult->getZeroTestsAreOk(),
        isForcibilyFailed => $suiteResult->isForcibilyFailed(),
    );

    my @tests = $suiteResult->getAllResults();
    foreach my $t (@tests)
    {
        $summary{testsCount}++;
        $summary{time} += $t->getTime();
        switch (uc($t->getState()))
        {
            case TestResult::SUCCESS { $summary{successCount}++; }
            case TestResult::FAILURE { $summary{failedCount}++; }
            case TestResult::ERROR { $summary{errorCount}++; $summary{success} = 0; }
            case TestResult::IGNORED { $summary{ignoredCount}++; $summary{notRunCount}++; }
            case TestResult::SKIPPED { $summary{skippedCount}++; $summary{notRunCount}++; }
            case TestResult::NOTRUNNABLE { $summary{notRunCount}++; }
            case TestResult::INCONCLUSIVE { $summary{inconclusiveCount}++; }
        }
    }

    return %summary;
}

1;
