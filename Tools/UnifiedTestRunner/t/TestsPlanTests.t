use Test::More;

use warnings;
use strict;
use Dirs;
use lib Dirs::external_root();
use Test::MockModule;

BEGIN { use_ok('TestPlan') }

use TestPlanItem;

can_ok('TestPlan', 'new');

Creation:
{
    my $plan = TestPlan->new();
    isa_ok($plan, 'TestPlan');
}

Register_TestItemWith3TestCases_ReturnsThreeAsTestCaseCountAndOneAsTestsCount:
{
    my $plan = TestPlan->new();
    can_ok($plan, 'registerTest');
    my $testPlanItem = new TestPlanItem(name => 'C.D.E', testCaseCount => 3);
    $plan->registerTest($testPlanItem);
    my $test = $plan->getTestItem('C.D.E');

    is($test->getName(), 'C.D.E');
    is($test->getTestCaseCount(), 3);
    my @tests = $plan->getAllTestItems();
    is(scalar(@tests), 1);
}

GetTotalTestCases_TwoTestItemsWithFiveTestCasesInTotal_ReturnsTotalNumberOfTestCases:
{
    my $plan = new TestPlan();
    my $testPlanItem1 = new TestPlanItem(name => 'A.B.C', testCaseCount => 2);
    my $testPlanItem2 = new TestPlanItem(name => 'C.D.E', testCaseCount => 3);
    $plan->registerTest($testPlanItem1);
    $plan->registerTest($testPlanItem2);
    my @tests = $plan->getAllTestItems();
    is(scalar(@tests), 2);
    is($plan->getTotalTestCases(), 5);
}

GetTestNames_ReturnsTestName:
{
    my $plan = new TestPlan();
    my $testPlanItem1 = new TestPlanItem(name => 'A.B.C', testCaseCount => 2);
    my $testPlanItem2 = new TestPlanItem(name => 'C.D.E', testCaseCount => 3);
    $plan->registerTest($testPlanItem1);
    $plan->registerTest($testPlanItem2);

    my @tests = $plan->getTestNames();
    is(scalar(@tests), 2);
    is($tests[0], 'A.B.C');
    is($tests[1], 'C.D.E');
}

Ignored_TestIsIncludedInTotal:
{
    my $plan = new TestPlan();
    my $testPlanItem1 = new TestPlanItem(name => 'A.B.C', testCaseCount => 0);
    $plan->registerTest($testPlanItem1);
    is($plan->getTotalTestCases(), 1);
    is($plan->getTotalIgnored(), 1);
}

CanLoadFromPlainTestListListFile:
{
    my $fileUtilsMock = new Test::MockModule('FileUtils');
    $fileUtilsMock->mock(
        readAllLines => sub
        {
            my ($fileName) = @_;
            if ($fileName eq 'path_to_test_list')
            {
                return ('T1', 'T2');
            }
            return undef;
        }
    );
    my $testPlan = TestPlan::loadFromPlainTestListFile('path_to_test_list');
    my @tests = map { $_->getName() } $testPlan->getAllTestItems();
    is_deeply(\@tests, ['T1', 'T2']);
}

done_testing();
