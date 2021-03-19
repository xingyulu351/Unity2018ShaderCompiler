use warnings;
use strict;

use TestResult;
use Test::More;
use TestHelpers::ArrayAssertions qw (arrayContains);
use SuiteResult;

BASIC:
{
    my $result = TestResult->new();
    is($result->isSucceeded(), 0, 'isSucceeded returns 0 for fresh result');
    is($result->isInconclusive(), 1, 'isInconclusive returns 1 for fresh result');

    $result = TestResult->new();
    $result->setState(TestResult::SUCCESS);
    is($result->isSucceeded(), 1, "isSucceeded returns 1 for succeeded result");

    $result = TestResult->new();
    is($result->isFailed(), 0, "isFailed returns 0 for fresh result");

    $result = TestResult->new();
    $result->setState(TestResult::FAILURE);
    is($result->isFailed(), 1, "isFailed returns 1 for succeeded result");

    $result->setState(TestResult::IGNORED);
    is($result->isIgnored(), 1, 'isIgnored returns 1 for ignored test');

    $result->setState(TestResult::SKIPPED);
    is($result->isSkipped(), 1, 'isSkipped returns 1 for skipped test');

    $result->setState(TestResult::INCONCLUSIVE);
    is($result->isInconclusive(), 1, 'isInconclusive returns 1 for inconclusive test');

    $result->setState(TestResult::ERROR);
    is($result->isError(), 1, 'isError returns 1 for error test');
}

TEST_STATE_TO_STRING:
{
    my $result = TestResult->new();
    $result->setState(TestResult::SUCCESS);
    is($result->getStateAsString(), 'SUCCESS');

    $result->setState(TestResult::INCONCLUSIVE);
    is($result->getStateAsString(), 'INCONCLUSIVE');

    $result->setState(TestResult::SKIPPED);
    is($result->getStateAsString(), 'SKIPPED');

    $result->setState(TestResult::IGNORED);
    is($result->getStateAsString(), 'IGNORED');

    $result->setState(TestResult::SUCCESS);
    is($result->getStateAsString(), 'SUCCESS');

    $result->setState(TestResult::FAILURE);
    is($result->getStateAsString(), 'FAILURE');

    $result->setState(TestResult::ERROR);
    is($result->getStateAsString(), 'ERROR');

    $result->setState(TestResult::NOTRUNNABLE);
    is($result->getStateAsString(), 'NOTRUNNABLE');
}

ARTIFACTS:
{
    my $result = TestResult->new();
    can_ok('TestResult', 'setMessage');
    can_ok('TestResult', 'getMessage');
    my $message = $result->getMessage();
    is($message, undef, 'Message is undefined by default');
    $result->setMessage('Epic failure');
    $message = $result->getMessage();
    is($message, 'Epic failure', 'Message is undefined by default');
}

ARTIFACTS:
{
    my $result = TestResult->new();
    $result->setArtifacts('a1', 'a2');
    my @artifacts = $result->getArtifacts();
    arrayContains(\@artifacts, ['a1', 'a2']);
}

PARENT_SUITE:
{
    my $result = TestResult->new();
    can_ok($result, 'setParent');
    my $suiteResult = SuiteResult->new(name => 'native', platform => 'standalone');
    $result->setParent($suiteResult);
    is($result->getSuite(), 'native');
    is($result->getPlatform(), 'standalone');
}

done_testing();
