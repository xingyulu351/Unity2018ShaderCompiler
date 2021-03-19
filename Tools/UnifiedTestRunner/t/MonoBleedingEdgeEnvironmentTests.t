use warnings;
use strict;

use Test::More;
use Dirs;
use File::Spec::Functions qw (canonpath);
use lib Dirs::external_root();
use Test::MockObject;
use TestHelpers::ArrayAssertions qw (arraysAreEqual arrayStartsWith arrayContains);
use Test::Deep 're';

my $root = Dirs::getRoot();
my $monoPrefix = canonpath("$root/External/MonoBleedingEdge/builds/monodistribution");
my $mono = canonpath("$monoPrefix/bin/mono");

BEGIN { use_ok('Environments::MonoBleedingEdge'); }

Creation:
{
    can_ok('Environments::MonoBleedingEdge', 'new');
    my $env = createEnv();
    isa_ok($env, 'Environments::MonoBleedingEdge');
}

getName_RetunsMonoeBe:
{
    my $env = new Environments::MonoBleedingEdge();
    is($env->getName(), 'monobe');
}

runDotNetProgram_BuildsCorrectCommandLine:
{
    my $env = createEnv();
    my @result = $env->runDotNetProgram(program => '/tmp/MyExe.exe', programArgs => ['arg1', 'arg2']);
    my $systemCall = $env->getSystemCall();
    my @args = $systemCall->getArgs();

    arraysAreEqual(\@args, [$mono, '/tmp/MyExe.exe', 'arg1', 'arg2']);
}

runDotNetProgram_MonoArgsAreSpecified_BuildsCorrectCommandLine:
{
    my $env = createEnv();
    my @result = $env->runDotNetProgram(program => 'MyExe.exe', monoArgs => ['--runtime=v4.0']);
    my $systemCall = $env->getSystemCall();
    my @args = $systemCall->getArgs();
    arraysAreEqual(\@args, [$mono, '--runtime=v4.0', 'MyExe.exe']);
}

runNUnitProject_BuildsCorrectCommandLine:
{
    my $env = createEnv();

    my $dll1 = canonpath('/tmp/somepath1/myproj.dll');
    my $dll2 = canonpath('/tmp/somepath2/myproj.dll');
    my $dll3 = canonpath('/tmp/somepath2/myproj1.dll');

    $env->runNUnitProject(dlls => [$dll1, $dll2, $dll3]);
    my $systemCall = $env->getSystemCall();
    my @args = $systemCall->getArgs();
    arrayStartsWith(\@args, [$mono, re(qr/nunit-console.exe/), '--encoding=utf-8', $dll1, $dll2, $dll3]);
}

runNUnitProject_MonoArgsAreSpecified_BuildsCorrectCommandLine:
{
    my $env = createEnv();

    my $dll1 = canonpath('/tmp/somepath1/myproj.dll');
    $env->runNUnitProject(dlls => [$dll1], monoArgs => ['monoArg1', 'monoArg2']);
    my $systemCall = $env->getSystemCall();
    my @args = $systemCall->getArgs();

    arrayStartsWith(\@args, [$mono, 'monoArg1', 'monoArg2', re(qr/nunit-console.exe/), '--encoding=utf-8', $dll1]);
}

runNUnitProject_NUnitArgsAreSpecified_BuildsCorrectCommandLine:
{
    my $env = createEnv();
    my $dll1 = canonpath('/tmp/somepath1/myproj.dll');
    $env->runNUnitProject(dlls => [$dll1], directRunnerArgs => ['runnerArg1', 'runnerArg2']);
    my $systemCall = $env->getSystemCall();
    my @args = $systemCall->getArgs();
    arrayStartsWith(\@args, [$mono, re(qr/nunit-console.exe/), '--encoding=utf-8', $dll1, 'runnerArg1', 'runnerArg2']);
}

done_testing();

sub createEnv
{
    my @args;
    my $systemMock = Test::MockObject->new();
    $systemMock->mock('executeAndWatchFiles', sub { shift(); my %p = @_; @args = @{ $p{command} } });
    $systemMock->mock('getArgs', sub { return @args; });
    my $env = new Environments::MonoBleedingEdge();
    $env->setSystemCall($systemMock);
    return $env;
}
