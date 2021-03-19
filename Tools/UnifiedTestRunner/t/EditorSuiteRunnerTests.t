use warnings;
use strict;

use Test::More;
use RunContext;

BEGIN
{
    use_ok('SuiteRunners::Editor');
}

Creation:
{
    my $runner = createRunner();
    isa_ok($runner, 'SuiteRunners::Editor');
}

defaultValues:
{
    my $r = createRunner();
    is($r->getName(), 'editor');
    is($r->getOptionValue('zero-tests-are-ok'), 1);
}

done_testing();

sub createRunner
{
    my $runner = new SuiteRunners::Editor();
    my $runContext = RunContext::new(new Options(options => $runner->{suiteOptions}));
    $runner->setRunContext($runContext);
    return $runner;
}
