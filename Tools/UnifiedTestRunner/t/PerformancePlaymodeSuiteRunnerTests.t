use warnings;
use strict;

use Test::More;
use File::Temp qw (tempdir);
use TestHelpers::Runners qw (runAndGrabExecuteInput);
use Dirs;
use lib Dirs::external_root();
use Test::MockObject;
use Test::MockObject::Extends;
use RunContext;

BEGIN
{
    use_ok('SuiteRunners::PerformancePlaymode');
}

Creation:
{
    my $runner = createRunner();

    isa_ok($runner, 'SuiteRunners::PerformancePlaymode');
}

defaultValues:
{
    my $r = createRunner();
    is($r->getName(), 'performance-playmode');
    is($r->getReporterSuiteName(), 'Playmode');
    is($r->getPlatform(), 'playmode');
    is($r->getOptionValue('platform'), 'playmode');
    is($r->getOptionValue('projectlist'), 'Tests/PerformanceTests/projectlist.txt');

}

run_InvokesPrepareTarget:
{
    my $r = createRunner();
    my $platform;
    $r->mock(
        prepareBuildTarget => sub
        {
            (undef, $platform) = @_;
        }
    );
    runAndGrabExecuteInput($r);
    is($platform, 'playmode');
}

run_PlatformIsSpecifiedInvokesPrepareTarget:
{
    my $r = createRunner();
    my $platform;
    $r->mock(
        prepareBuildTarget => sub
        {
            (undef, $platform) = @_;
        }
    );
    runAndGrabExecuteInput($r, '--platform=xbox');
    is($platform, 'xbox');
}

done_testing();

sub createRunner
{
    my $r = new SuiteRunners::PerformancePlaymode();
    $r = new Test::MockObject::Extends($r);
    my $options = new Options(options => $r->getOptions());
    my $runContext = RunContext::new($options);
    $r->setRunContext($runContext);
    $r->mock(
        _getProjectDirs => sub
        {
            (undef, undef) = @_;
        }
    );
    return $r;
}
