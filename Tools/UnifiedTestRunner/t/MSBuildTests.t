use warnings;
use strict;

use Test::More;
use File::Spec::Functions qw (canonpath);
use TestHelpers::ArrayAssertions qw (arrayStartsWith arrayContains arrayEndsWith arraysAreEqual);
use TestHelpers::ArgsGrabber;
use Environments::Win;
use File::Temp qw (tempfile);
use Dirs;
use lib Dirs::external_root();
use Test::MockModule;
use Test::Trap;
use Test::Output;

my $root = Dirs::getRoot();
my $utrRoot = Dirs::UTR_root();
my $project = canonpath("$utrRoot/t/TestData/csproj/dummy_proj.csproj");
my $msbuild = '"' . canonpath("c:/somewhere/MSBuild.exe") . '"';
my $config = "Debug";
my $verbosity = "quiet";
my $logfile = "build.log";

my $mercurialRepo = new Test::MockModule('FileWatcher');
$mercurialRepo->mock(new => sub { return bless({}, shift()); });
$mercurialRepo->mock(watch => sub { });
$mercurialRepo->mock(start => sub { });
$mercurialRepo->mock(stop => sub { });

BEGIN { use_ok('BuildEngines::MSBuild'); }

BASIC:
{
    can_ok('BuildEngines::MSBuild', 'new');
    my $builder = BuildEngines::MSBuild->new();
    isa_ok($builder, 'BuildEngines::MSBuild');
    can_ok('BuildEngines::MSBuild', 'build');
    is($builder->getName(), 'msbuild');
}

CAN_BUILD_CORRECT_COMMAND_LINE_FOR_PROJECT:
{
    my @cmdLineArgs = runBuilder(
        config => $config,
        verbosity => $verbosity,
        logfile => $logfile,
        project => $project,
        rebuild => 0
    );

    is(scalar(@cmdLineArgs), 7);

    arrayStartsWith(\@cmdLineArgs, $msbuild, $project);
    arrayContains(\@cmdLineArgs, ['/target:Build', "/property:Configuration=$config", "/verbosity:$verbosity", '/nologo']);
    arrayEndsWith(\@cmdLineArgs, "1>$logfile");
}

CAN_SPECIFY_FRAMEWORK_VERSION:
{
    my @cmdLineArgs = runBuilder(
        config => $config,
        verbosity => $verbosity,
        logfile => $logfile,
        project => $project,
        rebuild => 0,
        framework => 'A.B'
    );
    arrayContains(\@cmdLineArgs, '/property:TargetFrameworkVersion=A.B');
}

CAN_SPECIFY_ARGS_FOR_PROJECT:
{
    my @cmdLineArgs = runBuilder(
        config => $config,
        verbosity => $verbosity,
        logfile => $logfile,
        project => $project,
        project_args => '/property:DefineConstants=A;B',
        rebuild => 0
    );
    arrayContains(\@cmdLineArgs, '/property:DefineConstants=A;B');
}

CLEAN_PROJECT_BUILDS_CORRECT_COMMAND_LINE:
{
    my $builder = createBuilder();
    $builder->clean('A');
    my $systemCall = $builder->getSystemCall();
    my @args = $systemCall->getArgs();
    arraysAreEqual(\@args, [$msbuild, 'A', '/target:Clean', '/noconsolelogger', '/nologo']);
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

CAN_SPECIFY_OUTPUT_PATH:
{
    my @cmdLineArgs = runBuilder(
        config => $config,
        project => $project,
        outputPath => 'output_path'
    );
    arrayContains(\@cmdLineArgs, '/property:OutputPath=output_path');
    arrayContains(\@cmdLineArgs, '/property:BaseIntermediateOutputPath=' . canonpath('output_path/Intermediate') . canonpath('/'));
}

CAN_SPECIFY_PLATFORM_PATH:
{
    my @cmdLineArgs = runBuilder(
        config => $config,
        project => $project,
        platform => 'Win32'
    );
    arrayContains(\@cmdLineArgs, '/property:Platform=Win32');
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
                return 0;
            }
            @filesToWatch = @{ $p{filesToWatch} };
            return 0;
        }
    );

    $systemMock->mock('getArgs', sub { return @args; });
    $systemMock->mock('getFilesToWatch', sub { return @filesToWatch; });

    my $builder = new BuildEngines::MSBuild();
    $builder->setSystemCall($systemMock);
    $builder->setMSBuildPathProvider(sub { return "c:/somewhere/"; });
    return $builder;
}
