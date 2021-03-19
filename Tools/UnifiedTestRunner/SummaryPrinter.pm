package SummaryPrinter;

use warnings;
use strict;

use Logger;

use File::Spec::Functions qw (canonpath);

use List::MoreUtils qw (uniq);
use TestResult;
use SummaryBuilder;
use Term::ANSIColor qw( colored );
use Term::ANSIColor qw(:constants);
use File::Spec::Functions qw (catfile);
use StringUtils qw (isNullOrEmpty);

sub print
{
    my ($resultCollector, $runContext) = @_;
    printFailedTests($resultCollector);
    printHighLevelSummary($resultCollector, $runContext);
}

sub printFailedTests
{
    my ($resultCollector, $runContext) = @_;

    if (not isAnyTestFailed($resultCollector))
    {
        return;
    }

    my @suiteNames = getUniqueSuiteNames($resultCollector->getAllSuiteResults());
    foreach my $suiteName (@suiteNames)
    {
        my @allTestsInSuite = getAllTestsForSuite($resultCollector, $suiteName);
        my @failedTests = grep { $_->getState() eq TestResult::FAILURE or $_->getState() eq TestResult::ERROR or $_->getState() eq TestResult::INCONCLUSIVE }
            @allTestsInSuite;
        if (scalar(@failedTests) == 0 || $suiteName eq 'playmode' || $suiteName eq 'editor')
        {
            next;
        }
        Logger::minimal("Errors and Failures for $suiteName tests:");
        my (@testStrings);
        foreach my $test (@failedTests)
        {
            my $name = getTestName($test);
            my $state = TestResult::stateToString($test->getState());
            my $message = $test->getMessage();
            Logger::minimal(colored("$state: $name", 'bright_red'));
            if ($message)
            {
                Logger::minimal(colored("$message", 'bright_red'));
            }
        }
    }
}

sub printHighLevelSummary
{
    my ($resultCollector, $runContext) = @_;
    my %summary = buildSummary($resultCollector, $runContext);
    my $status = colored('PASS', 'bright_green');
    if (not $summary{success})
    {
        $status = colored('FAIL', 'bright_red');
    }

    Logger::minimal("Overall result: $status");

    if (keys %{ $summary{suiteSummaries} } > 1)
    {
        printSuiteSummary(%summary);
    }

    my $invalidCount = $summary{notRunCount} - $summary{ignoredCount} - $summary{skippedCount};
    my $failures = $summary{failedCount};
    if ($failures > 0)
    {
        $failures = colored("$failures", 'bright_red');
    }

    my $errors = $summary{errorCount};
    if ($errors > 0)
    {
        $errors = colored("$errors", 'bright_red');
    }

    Logger::minimal(
        "Total Tests run: $summary{testsCount}, Passed: $summary{successCount}, Failures: $failures, Errors: $errors, Inconclusives: $summary{inconclusiveCount}"
    );

    Logger::minimal("Total not run : $summary{notRunCount}, Invalid: $invalidCount, Ignored: $summary{ignoredCount}, Skipped: $summary{skippedCount}");

    my $compilationErrors = 0;
    if (defined($summary{compilationErrorsCount}))
    {
        if ($summary{compilationErrorsCount} != 0)
        {
            $compilationErrors = colored("$summary{compilationErrorsCount}", 'bright_red');
        }
        Logger::minimal("Compilation errors : $compilationErrors");
    }

    my $otherErrors = 0;
    if (defined($summary{otherErrorsCount}))
    {
        if ($summary{otherErrorsCount} != 0)
        {
            $otherErrors = colored("$summary{otherErrorsCount}", 'bright_red');
        }
        Logger::minimal("Other errors : $otherErrors");
    }

    my $instabilityCount = 0;
    if (defined($summary{instabilityCount}))
    {
        if ($summary{instabilityCount} != 0)
        {
            $instabilityCount = colored("$summary{instabilityCount}", 'yellow');
        }
        Logger::minimal("Instabilities : $instabilityCount");
    }

    my $nonTestRelatedInstabilityCount = 0;
    if (defined($summary{nonTestRelatedInstabilityCount}))
    {
        if ($summary{nonTestRelatedInstabilityCount} != 0)
        {
            $nonTestRelatedInstabilityCount = colored("$summary{nonTestRelatedInstabilityCount}", 'yellow');
        }
        Logger::minimal("Non Test Related Instabilities : $nonTestRelatedInstabilityCount");
    }

    if ($summary{failureConclusion})
    {
        my $conclussion = StringUtils::trim($summary{failureConclusion});
        Logger::minimal(colored("$summary{failureConclusion}", 'bright_red'));
    }

    if ($summary{successConclusion})
    {
        my $conclussion = StringUtils::trim($summary{successConclusion});
        Logger::minimal(colored("$summary{successConclusion}", 'bright_green'));
    }
}

sub buildSummary
{
    my ($resultCollector, $runContext) = @_;
    my $testReport = catfile($runContext->getArtifactsPath(), "TestReportData.json");
    if (-e $testReport)
    {
        my $report = FileUtils::readJsonObj($testReport);
        return %{ $report->{summary} };
    }
    my @suiteResults = $resultCollector->getAllSuiteResults();
    my %summary = SummaryBuilder::build(@suiteResults);
}

sub printSuiteSummary
{
    my (%summary) = @_;
    foreach my $suiteSummaryRef (values %{ $summary{suiteSummaries} })
    {
        my $suiteStatus = "PASS";
        if (not $suiteSummaryRef->{success})
        {
            $suiteStatus = "FAIL";
        }

        Logger::minimal("  Suite '"
                . $suiteSummaryRef->{name}
                . "' $suiteStatus; Tests run: "
                . $suiteSummaryRef->{testsCount}
                . ", Passed: "
                . $suiteSummaryRef->{successCount}
                . ", Failures: "
                . $suiteSummaryRef->{failedCount}
                . ", Errors: "
                . $suiteSummaryRef->{errorCount}
                . ", Inconclusives: "
                . $suiteSummaryRef->{inconclusiveCount});
    }
}

sub isAnyTestFailed
{
    my ($resultCollector) = @_;
    foreach my $result ($resultCollector->getAllResults())
    {
        if (not $result->isSucceeded())
        {
            return 1;
        }
    }
    return 0;
}

sub getAllTestsForSuite
{
    my ($resultCollector, $suiteName) = @_;
    my (@failedTests);
    foreach my $result ($resultCollector->getAllResults())
    {
        push(@failedTests, $result) if ($result->getSuite() eq $suiteName);
    }
    return @failedTests;
}

sub getTestName
{
    my ($testResult) = @_;
    my $fixture = $testResult->getFixture();
    my $name = $testResult->getTest();
    my $result = '';
    if ($fixture)
    {
        $result .= $fixture . '.';
    }
    $result .= $name;
}

sub getUniqueSuiteNames
{
    my (@suiteResults) = @_;

    my (@suiteNames);
    for my $suiteResult (@suiteResults)
    {
        my $suiteName = $suiteResult->getName();
        push(@suiteNames, $suiteName);
    }

    @suiteNames = uniq(@suiteNames);
}

sub printErrors
{
    my (@items) = @_;
    my $errorNo = 0;
    foreach my $item (@items)
    {
        my $fileName = canonpath($item->{fileName});
        my @errors = @{ $item->{errors} };
        foreach my $e (@errors)
        {
            my $errorMessage = StringUtils::trim($e->{errorMessage});
            my $lineNumber = $e->{logLineNumber};
            $errorNo++;
            Logger::minimal("Error #$errorNo");
            Logger::minimal(colored("\t$errorMessage", 'bright_red'));
            Logger::minimal(colored("\tIn $fileName:$lineNumber", 'bright_red'));
            if ($e->{callStackFound})
            {
                Logger::minimal("\tStack trace:");
                my @callStack = split("\n", $e->{errorCallStack});
                foreach my $item (@callStack)
                {
                    if (StringUtils::isNullOrEmpty($item))
                    {
                        next;
                    }
                    Logger::minimal(colored("\t$item", 'bright_red'));
                }
            }
        }
    }
}

1;
