package TestHelpers::TestPlan;

use warnings;
use strict;

use TestPlan;

sub createTestPlanFromTestObjects
{
    my (@tests) = @_;
    my $result = new TestPlan();
    foreach my $t (@tests)
    {
        my $testItem = new TestPlanItem(name => $t, testCaseCount => 1);
        $result->registerTest($testItem);
    }
    return $result;
}

1;
