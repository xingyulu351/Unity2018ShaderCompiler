use warnings;
use strict;

use Test::More;
use File::Spec::Functions qw (canonpath);
use TestHelpers::ArrayAssertions qw (arrayStartsWith arrayDoesNotContain arrayContains arrayEndsWith arraysAreEqual);
use TestHelpers::ArgsGrabber;

use Dirs;
use File::Temp qw (tempfile);
use lib Dirs::external_root();
use BuildEngines::XBuild;
use Environments::Mono;
use Test::MockModule;
use Test::MockObject::Extends;
use Test::Trap;
use Test::Deep 're';

my $root = Dirs::getRoot();
my $utrRoot = Dirs::UTR_root();
my $project = canonpath("$utrRoot/t/TestData/csproj/dummy_proj.csproj");
my $xbuild = canonpath("$root/External/MonoBleedingEdge/builds/monodistribution/bin/xbuild");
my $config = "Debug";
my $verbosity = "quiet";
my $log = "build.log";

my $mercurialRepo = new Test::MockModule('FileWatcher');
$mercurialRepo->mock(new => sub { return bless({}, shift()); });
$mercurialRepo->mock(watch => sub { });
$mercurialRepo->mock(start => sub { });
$mercurialRepo->mock(stop => sub { });

BASIC:
{
    my $builder = new BuildEngines::XBuild();
    is($builder->getName(), 'xbuild');
}

CAN_BUILD_CORRECT_COMMAND_LINE_FOR_PROJECT:
{
    my @cmdLineArgs = runBuilder(
        config => $config,
        verbosity => $verbosity,
        logfile => $log,
        project => $project
    );

    arrayStartsWith(\@cmdLineArgs, $xbuild, $project);

    arrayContains(\@cmdLineArgs, ['/target:Build', "/property:Configuration=$config", "/verbosity:$verbosity", "/nologo"]);
    arrayDoesNotContain(\@cmdLineArgs, '/property:BaseIntermediateOutputPath=output_path/Intermediate');
    arrayEndsWith(\@cmdLineArgs, "1>$log");
}

CAN_SPECIFY_FRAMEWORK_VERSION:
{
    my @cmdLineArgs = runBuilder(
        config => $config,
        verbosity => $verbosity,
        logfile => $log,
        project => $project,
        framework => 'A.B'
    );
    arrayContains(\@cmdLineArgs, '/property:TargetFrameworkVersion=A.B');
}

CAN_SPECIFY_ARGS_FOR_PROJECT:
{
    my @cmdLineArgs = runBuilder(
        config => $config,
        verbosity => $verbosity,
        logfile => $log,
        project => $project,
        project_args => '/property:DefineConstants=A;B'
    );
    arrayContains(\@cmdLineArgs, '/property:DefineConstants=A;B');
}

CLEAN_PROJECT_BUILDS_CORRECT_COMMAND_LINE:
{
    my $builder = createBuilder();
    $builder->clean('A');
    my $systemCall = $builder->getSystemCall();
    my @args = $systemCall->getArgs();
    arraysAreEqual(\@args, [$xbuild, 'A', '/target:Clean', '/noconsolelogger', '/nologo']);
}

CAN_SPECIFY_TARGET_FRAMEWROK_45:
{
    my @cmdLineArgs = runBuilder(
        config => $config,
        verbosity => $verbosity,
        logfile => $log,
        project => $project,
        framework => '4.5'
    );

    my $xbuild45 = canonpath("$root/External/MonoBleedingEdge/builds/monodistribution/lib/mono/4.5/xbuild.exe");
    arrayStartsWith(\@cmdLineArgs, [$xbuild45, $project]);
    arrayContains(\@cmdLineArgs, ['/property:TargetFrameworkVersion=4.5']);
    arrayEndsWith(\@cmdLineArgs, "1>$log");
}

#TODO: could be resued for both MSBUILD and XBUILD, when migration to Test::Class is done
CROAKS_ON_A_BUILD_ERROR:
{
    my $builder = createBuilder();
    my $systemMock = Test::MockObject->new();
    my ($fh, $logfile) = tempfile();
    close($fh);
    $systemMock->mock(
        'executeAndWatchFiles',
        sub
        {
            return 1;
        }
    );
    $builder->setSystemCall($systemMock);
    my @trap = trap
    {
        $builder->build(config => $config, project => $project, logfile => $logfile);
    };
    like($trap->die, qr/compilation.*failed/i);
}

CAN_SPCIFY_OUTPUT_PATH:
{
    my @cmdLineArgs = runBuilder(
        config => $config,
        project => $project,
        outputPath => 'output_path'
    );
    arrayContains(\@cmdLineArgs, '/property:OutputPath=output_path');
    arrayContains(\@cmdLineArgs, '/property:BaseIntermediateOutputPath=' . canonpath("output_path/Intermediate") . canonpath('/'));
}

done_testing();

sub runBuilder
{
    my %args = @_;
    my $builder = createBuilder();
    $builder->build(%args);
    my $systemCall = $builder->getSystemCall();
    return $systemCall->getArgs();
}

sub createBuilder
{
    my $systemMock = Test::MockObject->new();
    my @args;
    my @filesToWatch;
    $systemMock->mock(
        'executeAndWatchFiles',
        sub
        {
            shift();
            my %p = @_;
            @args = @{ $p{command} };
            if (not $p{filesToWatch})
            {
                return;
            }
            @filesToWatch = @{ $p{filesToWatch} };
            return 0;
        }
    );
    $systemMock->mock('getArgs', sub { return @args; });
    $systemMock->mock('getFilesToWatch', sub { return @filesToWatch; });
    my $builder = new BuildEngines::XBuild();
    $builder->setSystemCall($systemMock);
    return $builder;
}
