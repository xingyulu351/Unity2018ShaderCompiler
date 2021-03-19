use warnings;
use strict;

use Test::More;
use File::Temp qw (tempdir);
use TestHelpers::Runners qw (runAndGrabExecuteInput);
use File::Spec::Functions qw (canonpath);
use Dirs;
use lib Dirs::external_root();
use Test::MockObject;
use Test::MockObject::Extends;
use RunContext;

BEGIN
{
    use_ok('SuiteRunners::Playmode');
}

Creation:
{
    my $runner = createRunner();
    isa_ok($runner, 'SuiteRunners::Playmode');
}

defaultValues:
{
    my $r = createRunner();
    is($r->getName(), 'playmode');
    is($r->getPlatform(), 'playmode');
    is($r->getOptionValue('zero-tests-are-ok'), 1);
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
    my $r = new SuiteRunners::Playmode();
    $r = new Test::MockObject::Extends($r);
    my $options = new Options(options => $r->getOptions());
    my $runContext = RunContext::new($options);
    $r->setRunContext($runContext);
    $r->mock(_generateMSBuildScript => sub { });
    $r->mock(processResults => sub { });
    $r->mock(
        doRun => sub
        {
            my ($this) = @_;
            $this->{runnerExitCode} = 0;
            return 0;
        }
    );

    return $r;
}
