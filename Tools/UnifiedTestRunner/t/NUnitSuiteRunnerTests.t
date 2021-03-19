use warnings;
use strict;

use Test::More;
use NUnitSuiteRunnerBase;
use Environments::Mono;
use File::Spec::Functions qw (catfile);

use Dirs;
use lib Dirs::external_root();
use TestHelpers::ArrayAssertions qw (arraysAreEqual);
use TestHelpers::Runners qw (runAndGrabExecuteInput);
use TestHelpers::ArrayAssertions qw (arrayDoesNotContain);
use Test::Trap;
use Test::MockObject;
use Test::Deep 're';
use Test::MockObject::Extends;
use Test::MockModule;

buildNUnitTestProtocolAddin_ProducesCorrectCommandLine:
{
    my @cmdLine = build(0);
    my $program = catfile(Dirs::getRoot(), 'Tests', 'Unity.TestProtocol.NUnit', 'build');
    arraysAreEqual(\@cmdLine, [$program, re(qr/TestProtocol.NUnit.Addin-BuildLog.txt$/)]);
}

buildNUnitTestProtocolAddin_CroaksIfNUnitAddinBuildHasFaild:
{
    my @trap = trap
    {
        my @cmdLine = build(1);
    };
    like($trap->die, qr/failed/);
}

Run_EmptyTestPlan_DoesNotStartNUnitConsole:
{
    my $runner = makeRunner();
    my $testPlanReaderMock = new Test::MockModule('NUnitSuiteRunnerBase');
    $testPlanReaderMock->mock(
        readTestPlanFromFile => sub
        {
            return new TestPlan();
        }
    );
    my @input = runAndGrabExecuteInput($runner);
    arrayDoesNotContain(\@input, re(qr/nunit-console[.]exe/));
    ok($runner->{runnerExitCode} != 0);
}

done_testing();

sub build
{
    my ($exitCode) = @_;
    my $runner = makeRunner();
    my $systemCallMock = new Test::MockObject();
    my @cmdLine;
    $systemCallMock->mock(
        execute => sub
        {
            (undef, @cmdLine) = @_;
            return $exitCode;
        }
    );
    my $env = new Environments::Mono();
    $env->setSystemCall($systemCallMock);
    $runner->setEnvironment($env);
    $runner->buildNUnitTestProtocolAddin();
    return @cmdLine;
}

sub makeRunner
{
    my $runner = new NUnitSuiteRunnerBase();
    $runner = new Test::MockObject::Extends($runner);
    $runner->mock(
        getName => sub
        {
            return 'Fake';
        }
    );
    $runner->mock(
        getPlatform => sub
        {
            return 'Fake';
        }
    );
    $runner->mock(
        getDllNames => sub
        {
            return ();
        }
    );

    return $runner;
}
