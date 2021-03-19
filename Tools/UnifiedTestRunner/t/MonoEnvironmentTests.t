use warnings;
use strict;

use Test::More;
use File::Spec::Functions qw (canonpath);
use TestHelpers::ArrayAssertions qw (arraysAreEqual arrayStartsWith arrayContains);

use Dirs;
use lib Dirs::external_root();
use FileUtils qw (getDirName);
use Test::MockModule;
use TestHelpers::ArgsGrabber;
use File::Basename;
use VSSolution;
use Test::Deep;

my $root = Dirs::getRoot();
my $monoPrefix = canonpath("$root/External/Mono/builds/monodistribution");
my $mono = canonpath("$root/Tools/UnifiedTestRunner/mono");
my $dll1 = canonpath("/tmp/somepath1/myproj.dll");
my $dll2 = canonpath("/tmp/somepath2/myproj.dll");
my $dll3 = canonpath("/tmp/somepath2/myproj1.dll");

my $path_separator = ':';
if ($^O eq 'MSWin32')
{
    $path_separator = ';';
}

BASIC:
{
    use_ok('Environments::Mono');
    can_ok('Environments::Mono', 'new');
    can_ok('Environments::Mono', 'setSystemCall');
    my $env = Environments::Mono->new();
    isa_ok($env, 'Environments::Mono');
    can_ok('Environments::Mono', qw [getName runDotNetProgram]);
    is($env->getName(), 'mono');
}

runDotNetProgram_NoArgsAreSpecified_BuildsCorrectCommandLine:
{
    my $env = createEnv();
    my $monoModuleMock = new Test::MockModule('Environments::Mono');
    $monoModuleMock->mock(_resetMonoVars => sub { });
    my @result = $env->runDotNetProgram(program => '/tmp/MyExe.exe', programArgs => ['arg1', 'arg2']);
    my $systemCall = $env->getSystemCall();
    my @args = $systemCall->getArgs();
    arraysAreEqual(\@args, [$mono, '--debug', '/tmp/MyExe.exe', 'arg1', 'arg2']);
    is($ENV{'MONO_PATH'}, canonpath("/tmp") . $path_separator . canonpath("$monoPrefix/lib/mono/2.0"));
    is($ENV{'MONO_CFG_DIR'}, canonpath("$monoPrefix/etc"));
    is($ENV{'LD_LIBRARY_PATH'}, canonpath("$monoPrefix/lib"));
}

runNUnitProject_CreatesCorrectCommandLineAnd_SetsEnvVariablesProperly:
{
    my $env = createEnv();
    my $monoModuleMock = new Test::MockModule('Environments::Mono');
    $monoModuleMock->mock(_resetMonoVars => sub { });

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

    $env->runNUnitProject(dlls => [$dll1, $dll2, $dll3]);

    ok(scalar(@grabbedNUnitDlls) > 0);
    ok(scalar(@grabbedNUnitArgs) > 0);

    my $systemCall = $env->getSystemCall();
    my @outputArgs = $systemCall->getArgs();
    arrayStartsWith(\@outputArgs, [$mono, '--debug', re(qr/nunit-console.exe/), @grabbedNUnitDlls, @grabbedNUnitArgs]);

    my $mono_prefix = canonpath("$root/External/Mono/builds/monodistribution");
    my $libs = canonpath("$mono_prefix/lib/mono/2.0");

    my ($expectedMonoPath1, $expectedMonoPath2) = _getDirNames($dll1, $dll2);
    is($ENV{'MONO_PATH'}, $expectedMonoPath1 . $path_separator . $expectedMonoPath2 . $path_separator . $libs);
    is($ENV{'MONO_CFG_DIR'}, canonpath("$mono_prefix/etc"));
    is($ENV{'LD_LIBRARY_PATH'}, canonpath("$mono_prefix/lib"));
}

runNUnitProject_Solution_AddsProjectPathesToMonoPath:
{
    my $nunitcommandLinebuilder = new Test::MockModule('NUnitCommandLine');
    my $monoModuleMock = new Test::MockModule('Environments::Mono');
    $monoModuleMock->mock(_resetMonoVars => sub { });

    my $utrRoot = Dirs::UTR_root();
    my $solutionPath = canonpath("$utrRoot/t/TestData/VSSolution/VSSolution.sln");

    my $sln = new VSSolution($solutionPath);

    my $env = createEnv();
    $env->runNUnitProject(dlls => [$solutionPath]);

    my @expectedPathes = (canonpath("$utrRoot/t/TestData/AssemblyWithTests1/bin/Debug"), canonpath("$utrRoot/t/TestData/AssemblyWithTests2/bin/Debug"));

    my @pathesInMonoPath = split($path_separator, $ENV{'MONO_PATH'});
    arrayContains(\@pathesInMonoPath, \@expectedPathes);
}

runNUnitProject_MonoArgs_PutsThemToCommandLine:
{
    my $env = createEnv();
    $env->runNUnitProject(
        dlls => [$dll1],
        directRunnerArgs => ['-arg1', '-arg2'],
        monoArgs => ['--runtime=v4.0']
    );

    my $systemCall = $env->getSystemCall();
    my @args = $systemCall->getArgs();
    arrayStartsWith(\@args, [$mono, '--runtime=v4.0', re(qr/nunit-console.exe/)]);
}

runNUnitProject_AddsDomainNoneAndNoShadowByDefault:
{
    my $env = createEnv();
    $env->runNUnitProject(dlls => [$dll1]);
    my $systemCall = $env->getSystemCall();
    my @args = $systemCall->getArgs();
    arrayStartsWith(\@args, [$mono, '--debug', re(qr/nunit-console.exe/), '--encoding=utf-8', $dll1, '-domain=None']);
}

done_testing();

sub _getDirNames
{
    my (@filenames) = @_;
    my @results;

    foreach my $file (@filenames)
    {
        push(@results, getDirName($file));
    }
    return @results;
}

sub createEnv
{
    my @args;
    my $systemMock = Test::MockObject->new();
    $systemMock->mock('executeAndWatchFiles', sub { shift(); my %p = @_; @args = @{ $p{command} } });
    $systemMock->mock('getArgs', sub { return @args; });
    my $env = new Environments::Mono();
    $env->setSystemCall($systemMock);
    return $env;
}
