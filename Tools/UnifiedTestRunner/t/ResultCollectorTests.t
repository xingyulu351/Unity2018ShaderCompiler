use warnings;
use strict;

use Test::More;
use ResultCollector;
use TestResult;
use SuiteResult;

GetAllResults_ReturnsCorrectNumber:
{
    my $collector = createCollector();
    can_ok($collector, 'addSuiteResult');
    my $suiteResult = new SuiteResult();
    $suiteResult->addTestResult(new TestResult());
    $collector->addSuiteResult($suiteResult);
    is(scalar $collector->getAllResults(), 1);
    $suiteResult->addTestResult(new TestResult());
    is(scalar $collector->getAllResults(), 2);
}

AddSuiteResult_SuiteExistsInCollector_AddsIt:
{
    my $collector = createCollector();
    my $suiteResult1 = new SuiteResult(name => 'native', platform => 'editor');
    my $suiteResult2 = new SuiteResult(name => 'native', platform => 'editor');
    $collector->addSuiteResult($suiteResult1);
    $collector->addSuiteResult($suiteResult2);
    my @suiteResults = $collector->getAllSuiteResults();
    is($suiteResults[0]->getName(), 'native');
    is($suiteResults[1]->getName(), 'native');
}

done_testing();

sub createCollector
{
    return new ResultCollector();
}
