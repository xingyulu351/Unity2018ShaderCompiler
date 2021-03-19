use warnings;
use strict;

use Test::More;

use Dirs;
use lib Dirs::external_root();

use Test::MockModule;
use SuiteRunners;
use Test::MockObject::Extends;

my @runners = SuiteRunners::getRunners();
my @tools = SuiteTools::getTools();

foreach my $r (@runners, @tools)
{
    my $name = $r->getName();
    my %result = $r->usage();
    ok(ref(\%result) eq 'HASH');
    ok($result{description}, "$name must have a description");
    ok($result{examples}, "$name a running examples");
}

done_testing();
