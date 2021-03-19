use warnings;
use strict;

use Test::More;

use ResultPresenters::Simple;
use TestResult;
use SuiteResult;
use Dirs;

use lib Dirs::external_root();
use Test::MockObject::Extends;

PRESENT_RETURNS_EMPTY_STRING_FOR_FRESH_RESULT:
{
    my $presenter = ResultPresenters::Simple::new();
    my $result = TestResult->new();
    is($presenter->present($result), '', "present returns empty string for fresh result");
}

PRESENT_RETURNS_PROPER_STRINGS:
{
    my $presenter = ResultPresenters::Simple::new();
    my $result = TestResult->new();
    $result->setParent(SuiteResult->new(name => 'suite', platform => 'standalone'));
    $result->setFixture('fixture');
    $result->setTest('test');
    $result = new Test::MockObject::Extends($result);
    $result->mock(getStateAsString => sub { return "A" });
    $result->setState(TestResult::SUCCESS);
    is($presenter->present($result), 'A suite fixture test 0', "present returns proper string for succeeded result");
}

done_testing();
