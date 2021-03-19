use warnings;
use strict;

use Test::More;
use File::Spec::Functions qw (canonpath);
use Dirs;
use BuildEngines::MSBuild;
use TestHelpers::ArgsGrabber;
use TestHelpers::ArrayAssertions qw (arraysAreEqual);
use Test::MockModule;

my $root = Dirs::getRoot();

BEGIN { use_ok('Environments::Win') }

Creation:
{
    can_ok('Environments::Win', 'new');
    my $env = Environments::Win->new();
    isa_ok($env, 'Environments::Win');
    is($env->getName(), 'win');
}

runDotNetProgram_ProducesCorrectCommandLine:
{
    my $env = createEnv();
    my @result = $env->runDotNetProgram(program => 'MyExe.exe', programArgs => ['arg1', 'arg2']);
    my $systemCall = $env->getSystemCall();
    my @args = $systemCall->getArgs();
    arraysAreEqual(\@args, ['MyExe.exe', 'arg1', 'arg2']);
}

runNUnitProjectProducesCorrectCommandLine:
{
    my $env = createEnv();

    my $nunitcommandLinebuilder = new Test::MockModule('NUnitCommandLine');

    my @grabbedNUnitDlls;
    my @grabbedNUnitArgs;
    $nunitcommandLinebuilder->mock(
        build => sub
        {
            my (%args) = @_;
            @grabbedNUnitDlls = @{ $args{dlls} };
            @grabbedNUnitArgs = @{ $args{directRunnerArgs} };
            return ('nunit-console.exe', @grabbedNUnitDlls, @grabbedNUnitArgs);
        }
    );

    $env->runNUnitProject(
        dlls => ['myproj.dll'],
        directRunnerArgs => ['-arg1', '-arg2']
    );
    my $systemCall = $env->getSystemCall();
    my @outputArgs = $systemCall->getArgs();
    arraysAreEqual(\@outputArgs, ['nunit-console.exe', 'myproj.dll', '-arg1', '-arg2']);
}

done_testing();

sub createEnv
{
    my @args;
    my $systemMock = Test::MockObject->new();
    $systemMock->mock('executeAndWatchFiles', sub { shift(); my %p = @_; @args = @{ $p{command} } });
    $systemMock->mock('getArgs', sub { return @args; });
    my $env = new Environments::Win();
    $env->setSystemCall($systemMock);
    return $env;
}
