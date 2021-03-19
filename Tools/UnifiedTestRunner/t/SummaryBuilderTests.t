use warnings;
use strict;

use Test::More;

use TestResult;
use SuiteResult;

use File::Spec::Functions qw (catfile catdir);

BEGIN { use_ok('SummaryBuilder') }

Build_NoTestSuites_ReturnsZeroSummary:
{
    my %summary = SummaryBuilder::build();

    is_deeply(
        \%summary,
        {
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
        }
    );
}

Build_OneTestSuiteWithSuccesfullTest_ReturnsCorrectSummary:
{
    my $suiteResult = _makeSuiteResult(tests => [{ state => TestResult::SUCCESS },]);

    my %summary = SummaryBuilder::build($suiteResult);

    is_deeply(
        \%summary,
        {
            testsCount => 1,
            successCount => 1,
            errorCount => 0,
            failedCount => 0,
            time => 0,
            ignoredCount => 0,
            skippedCount => 0,
            inconclusiveCount => 0,
            notRunCount => 0,
            success => 1,
            result => TestResult::SUCCESS,
            hasSuiteWithNoTests => 0,
        }
    );
}

Build_OnTestSuiteWithFailedTest_ReturnsCorrectSummary:
{
    my $suiteResult = _makeSuiteResult(tests => [{ state => TestResult::FAILURE },]);

    my %summary = SummaryBuilder::build($suiteResult);

    is_deeply(
        \%summary,
        {
            testsCount => 1,
            successCount => 0,
            errorCount => 0,
            failedCount => 1,
            time => 0,
            ignoredCount => 0,
            skippedCount => 0,
            inconclusiveCount => 0,
            notRunCount => 0,
            success => 0,
            result => TestResult::FAILURE,
            hasSuiteWithNoTests => 0,
        }
    );
}

Build_TestSuiteWithErrorTest_ReturnsCorrectSummary:
{
    my $suiteResult = _makeSuiteResult(tests => [{ state => TestResult::ERROR },]);

    my %summary = SummaryBuilder::build($suiteResult);

    is_deeply(
        \%summary,
        {
            testsCount => 1,
            successCount => 0,
            errorCount => 1,
            failedCount => 0,
            time => 0,
            ignoredCount => 0,
            skippedCount => 0,
            inconclusiveCount => 0,
            notRunCount => 0,
            success => 0,
            result => TestResult::FAILURE,
            hasSuiteWithNoTests => 0,
        }
    );
}

Build_TestSuiteWithIgnoredTest_ReturnsCorrectSummary:
{
    my $suiteResult = _makeSuiteResult(tests => [{ state => TestResult::IGNORED },]);

    my %summary = SummaryBuilder::build($suiteResult);

    is_deeply(
        \%summary,
        {
            testsCount => 1,
            successCount => 0,
            errorCount => 0,
            failedCount => 0,
            time => 0,
            ignoredCount => 1,
            skippedCount => 0,
            inconclusiveCount => 0,
            notRunCount => 1,
            success => 0,
            hasSuiteWithNoTests => 1,
            result => TestResult::INCONCLUSIVE,
        }
    );
}

Build_TestSuiteWithOneSkippedTest_ReturnsCorrectSummary:
{
    my $suiteResult = _makeSuiteResult(tests => [{ state => TestResult::SKIPPED },]);

    my %summary = SummaryBuilder::build($suiteResult);

    is_deeply(
        \%summary,
        {
            testsCount => 1,
            successCount => 0,
            errorCount => 0,
            failedCount => 0,
            time => 0,
            ignoredCount => 0,
            skippedCount => 1,
            inconclusiveCount => 0,
            notRunCount => 1,
            success => 0,
            hasSuiteWithNoTests => 1,
            result => TestResult::INCONCLUSIVE
        }
    );
}

Build_TestSuiteWithNotRunnableTest_ReturnsCorrectSummary:
{
    my $suiteResult = _makeSuiteResult(tests => [{ state => TestResult::NOTRUNNABLE },]);

    my %summary = SummaryBuilder::build($suiteResult);

    is_deeply(
        \%summary,
        {
            testsCount => 1,
            successCount => 0,
            errorCount => 0,
            failedCount => 0,
            time => 0,
            ignoredCount => 0,
            skippedCount => 0,
            inconclusiveCount => 0,
            notRunCount => 1,
            success => 0,
            hasSuiteWithNoTests => 1,
            result => TestResult::INCONCLUSIVE
        }
    );
}

Build_OneFailedOneSuccessfullTest_ReturnsCorrectSummary:
{
    my $suiteResult = _makeSuiteResult(tests => [{ state => TestResult::FAILURE }, { state => TestResult::SUCCESS }, { state => TestResult::ERROR },]);

    my %summary = SummaryBuilder::build($suiteResult);

    is_deeply(
        \%summary,
        {
            testsCount => 3,
            successCount => 1,
            errorCount => 1,
            failedCount => 1,
            time => 0,
            ignoredCount => 0,
            skippedCount => 0,
            inconclusiveCount => 0,
            notRunCount => 0,
            success => 0,
            result => TestResult::FAILURE,
            hasSuiteWithNoTests => 0,
        }
    );
}

Build_OneIgnoredOneSkippedAndOneSuccessfullTest_ReturnsCorrectSummary:
{
    my $suiteResult = _makeSuiteResult(tests => [{ state => TestResult::SUCCESS }, { state => TestResult::IGNORED }, { state => TestResult::SKIPPED },]);

    my %summary = SummaryBuilder::build($suiteResult);

    is_deeply(
        \%summary,
        {
            testsCount => 3,
            successCount => 1,
            errorCount => 0,
            failedCount => 0,
            time => 0,
            ignoredCount => 1,
            skippedCount => 1,
            inconclusiveCount => 0,
            notRunCount => 2,
            success => 1,
            result => TestResult::SUCCESS,
            hasSuiteWithNoTests => 0,
        }
    );
}

Build_TwoSuitesWithSuccessfulTests_ReturnsCorrectSummary:
{
    my $suiteResult1 = _makeSuiteResult(tests => [{ state => TestResult::SUCCESS },]);
    my $suiteResult2 = _makeSuiteResult(tests => [{ state => TestResult::SUCCESS },]);

    my %summary = SummaryBuilder::build($suiteResult1, $suiteResult2);

    is_deeply(
        \%summary,
        {
            testsCount => 2,
            successCount => 2,
            errorCount => 0,
            failedCount => 0,
            time => 0,
            ignoredCount => 0,
            skippedCount => 0,
            inconclusiveCount => 0,
            notRunCount => 0,
            success => 1,
            result => TestResult::SUCCESS,
            hasSuiteWithNoTests => 0,
        }
    );
}

Build_OneTestSuiteWithInconclusiveTest_ReturnsCorrectSummary:
{
    my $suiteResult = _makeSuiteResult(tests => [{ state => TestResult::INCONCLUSIVE },]);

    my %summary = SummaryBuilder::build($suiteResult);

    is_deeply(
        \%summary,
        {
            testsCount => 1,
            successCount => 0,
            errorCount => 0,
            failedCount => 0,
            time => 0,
            ignoredCount => 0,
            skippedCount => 0,
            inconclusiveCount => 1,
            notRunCount => 0,
            success => 0,
            result => TestResult::INCONCLUSIVE,
            hasSuiteWithNoTests => 0,
        }
    );
}

Build_OneTestSuiteWithInconclusiveTestAndSuccesfullTests_ReturnsCorrectSummary:
{
    my $suiteResult = _makeSuiteResult(tests => [{ state => TestResult::INCONCLUSIVE }, { state => TestResult::SUCCESS },]);

    my %summary = SummaryBuilder::build($suiteResult);

    is_deeply(
        \%summary,
        {
            testsCount => 2,
            successCount => 1,
            errorCount => 0,
            failedCount => 0,
            time => 0,
            ignoredCount => 0,
            skippedCount => 0,
            inconclusiveCount => 1,
            notRunCount => 0,
            success => 0,
            result => TestResult::FAILURE,
            hasSuiteWithNoTests => 0,
        }
    );
}

Build_ZeroRunableTestsAndZeroTestsAreOk_ReturnsCorrectSummary:
{
    my $suiteResult = _makeSuiteResult(tests => [{ state => TestResult::IGNORED }, { state => TestResult::SKIPPED },]);
    $suiteResult->setZeroTestsAreOk(1);

    my %summary = SummaryBuilder::build($suiteResult);

    is_deeply(
        \%summary,
        {
            testsCount => 2,
            successCount => 0,
            errorCount => 0,
            failedCount => 0,
            time => 0,
            ignoredCount => 1,
            skippedCount => 1,
            inconclusiveCount => 0,
            notRunCount => 2,
            success => 0,
            result => TestResult::INCONCLUSIVE,
            hasSuiteWithNoTests => 1,
        }
    );
}

Build_TwoNotRunnableTestsAndOneFailed_ReturnsCorrectSummary:
{
    my $suiteResult = _makeSuiteResult(tests => [{ state => TestResult::IGNORED }, { state => TestResult::SKIPPED }, { state => TestResult::FAILURE }]);
    $suiteResult->setZeroTestsAreOk(1);

    my %summary = SummaryBuilder::build($suiteResult);

    is_deeply(
        \%summary,
        {
            testsCount => 3,
            successCount => 0,
            errorCount => 0,
            failedCount => 1,
            time => 0,
            ignoredCount => 1,
            skippedCount => 1,
            inconclusiveCount => 0,
            notRunCount => 2,
            success => 0,
            result => TestResult::FAILURE,
            hasSuiteWithNoTests => 0,
        }
    );
}

Build_OneSuiteWithZeroRunableTestsAndOneSuiteWithOneRunableTest_ReturnsCorrectSummary:
{
    my $suiteResult1 = _makeSuiteResult(tests => [{ state => TestResult::IGNORED }, { state => TestResult::SKIPPED }]);
    $suiteResult1->setZeroTestsAreOk(1);

    my $suiteResult2 = _makeSuiteResult(tests => [{ state => TestResult::SUCCESS }]);
    $suiteResult2->setZeroTestsAreOk(1);

    my %summary = SummaryBuilder::build($suiteResult1, $suiteResult2);

    is_deeply(
        \%summary,
        {
            testsCount => 3,
            successCount => 1,
            errorCount => 0,
            failedCount => 0,
            time => 0,
            ignoredCount => 1,
            skippedCount => 1,
            inconclusiveCount => 0,
            notRunCount => 2,
            success => 1,
            result => TestResult::SUCCESS,
            hasSuiteWithNoTests => 1,
        }
    );
}

Build_SuiteResultForciblyFailed_ReturnsCorrectSummary:
{
    my $suiteResult = _makeSuiteResult(tests => [{ state => TestResult::SUCCESS }]);
    $suiteResult->forceFailed('failure');

    my %summary = SummaryBuilder::build($suiteResult);

    is_deeply(
        \%summary,
        {
            testsCount => 1,
            successCount => 1,
            errorCount => 0,
            failedCount => 0,
            time => 0,
            ignoredCount => 0,
            skippedCount => 0,
            inconclusiveCount => 0,
            notRunCount => 0,
            success => 0,
            result => TestResult::FAILURE,
            hasSuiteWithNoTests => 0,
        }
    );
}

Build_SuiteResultForciblyFailedWithZeroTestsFails_ReturnsCorrectSummary:
{
    my $suiteResult = _makeSuiteResult(tests => []);
    $suiteResult->forceFailed('failure');

    my %summary = SummaryBuilder::build($suiteResult);

    is_deeply(
        \%summary,
        {
            testsCount => 0,
            successCount => 0,
            errorCount => 0,
            failedCount => 0,
            time => 0,
            ignoredCount => 0,
            skippedCount => 0,
            inconclusiveCount => 0,
            notRunCount => 0,
            success => 0,
            result => TestResult::FAILURE,
            hasSuiteWithNoTests => 1,
        }
    );
}

Build_ZeroTestsAreOk_ReturnsCorrectSummary:
{
    my $suiteResult = _makeSuiteResult();
    $suiteResult->setZeroTestsAreOk(1);

    my %summary = SummaryBuilder::build($suiteResult);

    is_deeply(
        \%summary,
        {
            testsCount => 0,
            successCount => 0,
            errorCount => 0,
            failedCount => 0,
            time => 0,
            ignoredCount => 0,
            skippedCount => 0,
            inconclusiveCount => 0,
            notRunCount => 0,
            success => 0,
            result => TestResult::INCONCLUSIVE,
            hasSuiteWithNoTests => 1,
        }
    );
}

Build_MultipleSuitesOneWithNoTests_ExpectedSuiteSummaries:
{
    my $suiteResult1 = _makeSuiteResult(tests => [{ state => TestResult::SKIPPED }]);
    my $suiteResult2 = _makeSuiteResult(tests => [{ state => TestResult::SUCCESS }]);
    my $suiteResult3 = _makeSuiteResult();

    my @suiteResults = ($suiteResult1, $suiteResult2, $suiteResult3);

    my %summary = SummaryBuilder::build(@suiteResults);

    is_deeply(
        \%summary,
        {
            testsCount => 2,
            successCount => 1,
            errorCount => 0,
            failedCount => 0,
            time => 0,
            ignoredCount => 0,
            skippedCount => 1,
            inconclusiveCount => 0,
            notRunCount => 1,
            hasSuiteWithNoTests => 1,
            success => 0,
            result => TestResult::FAILURE
        }
    );
}

Build_RealityCheck1:
{
    my $tests = [
        { state => TestResult::NOTRUNNABLE, time => 1 },
        { state => TestResult::SUCCESS, time => 1 },
        { state => TestResult::ERROR, time => 1 },
        { state => TestResult::SUCCESS, time => 1 },
        { state => TestResult::IGNORED, time => 1 },
        { state => TestResult::INCONCLUSIVE, time => 1 },
        { state => TestResult::FAILURE, time => 1 },
        { state => TestResult::FAILURE, time => 1 },
        { state => TestResult::SUCCESS, time => 1 },
        { state => TestResult::SKIPPED, time => 1 },
        { state => TestResult::SUCCESS, time => 1 },
    ];

    my $suiteResult1 = _makeSuiteResult(tests => $tests);

    my $suiteResult2 = _makeSuiteResult(tests => $tests);

    my %summary = SummaryBuilder::build($suiteResult1, $suiteResult2);

    is_deeply(
        \%summary,
        {
            testsCount => 11 * 2,
            successCount => 4 * 2,
            errorCount => 1 * 2,
            failedCount => 2 * 2,
            time => 11 * 2,
            ignoredCount => 1 * 2,
            skippedCount => 1 * 2,
            inconclusiveCount => 1 * 2,
            notRunCount => 3 * 2,
            hasSuiteWithNoTests => 0 * 2,
            success => 0,
            result => TestResult::FAILURE
        }
    );
}

done_testing();

sub _makeSuiteResult
{
    my %args = @_;
    my $result = new SuiteResult();
    if (exists $args{name})
    {
        $result->setName($args{name});
    }

    if (exists $args{tests})
    {
        foreach my $t (@{ $args{tests} })
        {
            $result->addTestResult(_makeTestResult(%{$t}));
        }
    }

    return $result;
}

sub _makeTestResult
{
    my %args = @_;
    my $result = new TestResult();
    if (exists $args{time})
    {
        $result->setTime($args{time});
    }

    $result->setState($args{state});
    return $result;
}
