use warnings;
use strict;

use Test::More;

use Dirs;
use lib Dirs::external_root();
use Test::MockObject;
use Test::Trap;
use Test::MockObject::Extends;
use File::Spec::Functions qw (canonpath);
use TestHelpers::Runners qw (runAndGrabExecuteInput makeDefaultSuiteRunnerArtifactNameBuilderStub runAndGrabBuildEngineInput);
use TestHelpers::ArrayAssertions qw (arrayContains arraysAreEqual);
use CSProj;
use VSSolution;
use Environments::MonoBleedingEdge;
use Test::Deep 're';

my $utrRoot = Dirs::UTR_root();
my $proj1 = canonpath("$utrRoot/t/TestData/AssemblyWithTests1/AssemblyWithTests1.csproj");
my $proj2 = canonpath("$utrRoot/t/TestData/AssemblyWithTests2/AssemblyWithTests2.csproj");
my $proj1Dll = new CSProj($proj1)->getAssemblyFullPath();
my $proj2Dll = new CSProj($proj2)->getAssemblyFullPath();

my $testPlanReaderMock = new Test::MockModule('NUnitSuiteRunnerBase');
$testPlanReaderMock->mock(
    readTestPlanFromFile => sub
    {
        my $result = new TestPlan();
        $result->registerTest(new TestPlanItem(name => 'foo', testCaseCount => 1));
        return $result;
    }
);

BEGIN { use_ok('NUnitConfigBasedSuiteRunner'); }

Creation:
{
    my $runner = makeRunnerMock();
    isa_ok($runner, 'NUnitConfigBasedSuiteRunner');
}

getName_NameIsSpecifiedInCofig_ReturnsIt:
{
    my $config = { name => 'suite_name' };
    my $runner = makeRunnerMock($config);
    is($runner->getName(), 'suite_name');
}

getPlatform_PlatformIsSpecifiedInCofig_ReturnsIt:
{
    my $config = { platform => 'platform_name' };
    my $runner = makeRunnerMock($config);
    is($runner->getPlatform(), 'platform_name');
}

#TODO: croak if name is not specified
run_CSPorjectsAreSpecifiedAsInputfiles_ConvertsThemToDlls:
{
    my @nunitInputFiles = ['inputfile1.dll', $proj1, $proj2];
    my $config = {
        name => 'suite_name',
        platform => 'suite_platform',
        nunitInputFiles => @nunitInputFiles,
    };
    my $runner = makeRunnerMock($config);

    my @input = runAndGrabExecuteInput($runner);
    arrayContains(\@input, [re(qr/nunit-console.exe/), 'inputfile1.dll', $proj1Dll, $proj2Dll]);
}

run_DisableResolveDlls_UseInputFilesAsIs:
{
    my $solutionPath = canonpath("$utrRoot/t/TestData/VSSolution/VSSolution.sln");
    my @nunitInputFiles = ['inputfile1.dll', $proj1, $proj2, $solutionPath];
    my $config = {
        name => 'suite_name',
        nunitInputFiles => @nunitInputFiles,
        disableResolveDlls => 1
    };

    my $runner = makeRunnerMock($config);
    my @input = runAndGrabExecuteInput($runner);
    arrayContains(\@input, [re(qr/nunit-console.exe/), 'inputfile1.dll', $proj1, $proj2, $solutionPath]);
}

run_SameCSProjectSpecifiedTwice_InPutFileContainsOnlyUniqueDlls:
{
    my $solutionPath = canonpath("$utrRoot/t/TestData/VSSolution/VSSolution.sln");
    my @nunitInputFiles = [$proj1, $proj1];
    my $config = {
        name => 'suite_name',
        nunitInputFiles => @nunitInputFiles
    };
    my $runner = makeRunnerMock($config);
    my @input = runAndGrabExecuteInput($runner);
    my %params;
    for (@input) { $params{$_}++ }
    is($params{$proj1Dll}, 1);
}

run_VSSolutionIsSpecifiedAsInputfile:
{
    my $solutionPath = canonpath("$utrRoot/t/TestData/VSSolution/VSSolution.sln");
    my @nunitInputFiles = [$solutionPath];
    my $config = {
        name => 'suite_name',
        platform => 'suite_platform',
        nunitInputFiles => @nunitInputFiles,
    };
    my $runner = makeRunnerMock($config);
    my $sln = new VSSolution($solutionPath);
    my @input = runAndGrabExecuteInput($runner);
    arrayContains(\@input, $proj1Dll, $proj2Dll);
}

run_CSBuildDependenciesAreSpecified_BuildsThem:
{
    my $nunitInputFiles = ['inputfile1.dll', 'inputfile1.dll'];
    my $dependencies = [$proj1, $proj2];
    my $config = {
        name => 'suite_name',
        platform => 'suite_platform',
        nunitInputFiles => $nunitInputFiles,
        csDependencies => $dependencies
    };

    my $runner = makeRunnerMock($config);
    $runner = TestHelpers::Runners::makeRunnerMock($runner);

    my @grabbedProjects;
    $runner->mock(
        'buildCSProject' => sub
        {
            my (undef, $proj) = @_;
            push(@grabbedProjects, $proj);
        }
    );

    $runner->run();
    arraysAreEqual(\@grabbedProjects, [re(qr/TestLister[.]csproj/i), $proj1, $proj2]);
}

run_TargetFrameworkVersionIsSpecified_ReturnsIt:
{
    my $nunitInputFiles = ['inputfile1.dll', 'inputfile2.dll'];
    my $dependencies = [$proj1];
    my $config = {
        name => 'suite_name',
        platform => 'suite_platform',
        nunitInputFiles => $nunitInputFiles,
        csDependencies => $dependencies,
        targetFrameworkVersion => '4.5'
    };

    my $runner = makeRunnerMock($config);
    my (%builderArgs) = runAndGrabBuildEngineInput($runner);
    is($builderArgs{framework}, '4.5');
}

run_RuntimeVersionIsSpecified_AddsItToCommandLine:
{
    my $nunitInputFiles = ['inputfile1.dll', 'inputfile2.dll'];
    my $dependencies = [$proj1];
    my $config = {
        name => 'suite_name',
        platform => 'suite_platform',
        nunitInputFiles => $nunitInputFiles,
        csDependencies => $dependencies,
        monoArgs => ['--runtime=4.0']
    };
    my $runner = makeRunnerMock($config);
    my @input = runAndGrabExecuteInput($runner);
    arrayContains(\@input, [re(qr/mono/), '--runtime=4.0']);
}

run_CSPorjectsAreSpecifiedAsInputfiles_ConvertsThemToDlls:
{
    my @nunitInputFiles = ['inputfile1.dll', $proj1, $proj2];
    my $config = {
        name => 'suite_name',
        platform => 'suite_platform',
        nunitInputFiles => @nunitInputFiles,
    };
    my $runner = makeRunnerMock($config);

    my @input = runAndGrabExecuteInput($runner);
    arrayContains(\@input, [re(qr/nunit-console.exe/), $proj1Dll, $proj2Dll]);
}

run_CleanupProjectisSpecified_InvokesCleanupOnIt:
{
    my $nunitInputFiles = ['inputfile1.dll', 'inputfile2.dll'];
    my $projectsToCleanup = [$proj1, $proj2];
    my $config = {
        name => 'suite_name',
        platform => 'suite_platform',
        nunitInputFiles => $nunitInputFiles,
        csProjectToCleanup => $projectsToCleanup
    };

    my $runner = makeRunnerMock($config);
    $runner = TestHelpers::Runners::makeRunnerMock($runner);

    my @grabbedProjects;
    $runner->mock(
        'cleanCSProject' => sub
        {
            my (undef, $proj) = @_;
            push(@grabbedProjects, $proj);
        }
    );

    $runner->run();
    arraysAreEqual(\@grabbedProjects, [$proj1, $proj2]);
}

run_DoesNotBuildDependenciesTwice:
{
    my $nunitInputFiles = ['inputfile1.dll'];
    my $dependencies = [$proj1, $proj2];
    my $config = {
        name => 'suite_name',
        platform => 'suite_platform',
        nunitInputFiles => $nunitInputFiles,
        csDependencies => $dependencies
    };

    my $runner = makeRunnerMock($config);
    $runner = TestHelpers::Runners::makeRunnerMock($runner);

    my $invokesCount = 0;
    $runner->mock(
        'buildCSProject' => sub
        {
            $invokesCount++;
        }
    );

    $runner->run();

    # -1 to substract test lister csproj
    is($invokesCount - 1, scalar(@{$dependencies}));
}

run_PlatformNotSpecified_SetsItToName:
{
    my $config = {
        name => 'suite_name',
        nunitInputFiles => ['inputfile1.dll'],
    };

    my $runner = makeRunnerMock($config);
    is($runner->getPlatform(), 'suite_name');
}

run_CroaksIfSuiteNameIsNotSpecified:
{
    my $config = {

        #name => 'suite_name',
        platform => 'suite_platform',
        nunitInputFiles => ['inputfile1.dll'],
    };

    my $runner = makeRunnerMock($config);
    $runner->mock(cleanPreviousLogsIfAny => sub { });
    my @trap = trap
    {
        runAndGrabExecuteInput($runner);
    };
    like($trap->die, qr/name/);
}

run_CroaksIfThereAreNoInputfiles:
{
    my $config = {
        name => 'suite_name',
        platform => 'suite_platform'

#       nunitInputFiles => ['inputfile1.dll'],
    };

    my $runner = makeRunnerMock($config);
    my @trap = trap
    {
        runAndGrabExecuteInput($runner);
    };
    like($trap->die, qr/nunitInputFiles/);
}

run_NUnitArgsAreSpecifiedPutsThemToCommandLine:
{
    my $nunitArgs = ['-trace=Verbose', '-stoponerror'];
    my $config = {
        name => 'suite_name',
        platform => 'suite_platform',
        nunitInputFiles => ['inputfile1.dll'],
        nunitArgs => $nunitArgs
    };

    my $runner = makeRunnerMock($config);
    my @input = runAndGrabExecuteInput($runner);
    arrayContains(\@input, $nunitArgs);
}

run_InvokesBuildOnInputSolutionAndCSProjFiles:
{
    my $solutionPath = canonpath("$utrRoot/t/TestData/VSSolution/VSSolution.sln");
    my @nunitInputFiles = [$solutionPath, $proj1];
    my $config = {
        name => 'suite_name',
        platform => 'suite_platform',
        nunitInputFiles => @nunitInputFiles
    };

    my $runner = makeRunnerMock($config);

    $runner = TestHelpers::Runners::makeRunnerMock($runner);

    my @grabbedProjects;
    $runner->mock(
        'buildCSProject' => sub
        {
            my (undef, $proj) = @_;
            push(@grabbedProjects, $proj);
        }
    );

    $runner->run();
    arraysAreEqual(\@grabbedProjects, [re(qr/TestLister.csproj/), $solutionPath, $proj1]);
}

run_ProjectFilterIsSpecified_BuildsTestListOnlyFromThoseProjects:
{
    my $solutionPath = canonpath("$utrRoot/t/TestData/VSSolution/VSSolution.sln");
    my @nunitInputFiles = [$solutionPath, $proj1];
    my $config = {
        name => 'suite_name',
        platform => 'suite_platform',
        nunitInputFiles => @nunitInputFiles
    };

    my $runner = makeRunnerMock($config);

    $runner = TestHelpers::Runners::makeRunnerMock($runner);

    my @grabbedProjects;
    $runner->mock(
        'buildCSProject' => sub
        {
            my (undef, $proj) = @_;
            push(@grabbedProjects, $proj);
        }
    );

    $runner->run();
    arraysAreEqual(\@grabbedProjects, [re(qr/TestLister.csproj/), $solutionPath, $proj1]);
}

run_IncludesExcludesAreSpecified_PutsItToListerCommadLine:
{
    my $config = {
        name => 'suite_name',
        nunitInputFiles => ['inputfile1.dll'],
        includes => ['Attribute1', 'Attribute2']
    };

    my $runner = makeRunnerMock($config);
    my @listerArgs = @_;
    $runner->mock(
        runDotNetProgram => sub
        {
            my (undef, %args) = @_;
            my $program = $args{program};
            if ($program =~ m/Lister[.]exe$/)
            {
                @listerArgs = @{ $args{programArgs} };
            }
        }
    );

    $runner = TestHelpers::Runners::makeRunnerMock($runner);
    $runner->run();
    arrayContains(\@listerArgs, ['--includes=Attribute1,Attribute2']);
}

run_ExcludesAreSpecified_PutsItToListerCommadLine:
{
    my $config = {
        name => 'suite_name',
        nunitInputFiles => ['inputfile1.dll'],
        excludes => ['Attribute1', 'Attribute2']
    };

    my $runner = makeRunnerMock($config);
    my @listerArgs = @_;
    $runner->mock(
        runDotNetProgram => sub
        {
            my (undef, %args) = @_;
            my $program = $args{program};
            if ($program =~ m/Lister[.]exe$/)
            {
                @listerArgs = @{ $args{programArgs} };
            }
        }
    );

    $runner = TestHelpers::Runners::makeRunnerMock($runner);
    $runner->run();
    arrayContains(\@listerArgs, ['--excludes=Attribute1,Attribute2']);
}

run_PrepareEnvIsSpecified_InvokesIt:
{
    my $prepareEnvInvoked = 0;
    my $config = {
        name => 'suite_name',
        nunitInputFiles => ['inputfile1.dll'],
        prepareEnv => sub { $prepareEnvInvoked = 1; }
    };

    my $runner = makeRunnerMock($config);
    $runner = TestHelpers::Runners::makeRunnerMock($runner);
    $runner->run();
    is($prepareEnvInvoked, 1);
}

run_EnvironmentIsNotSpecified_UsesMono:
{
    my $prepareEnvInvoked = 0;
    my $config = {
        name => 'suite_name',
        nunitInputFiles => ['inputfile1.dll']
    };
    my $runner = makeRunnerMock($config);
    $runner = TestHelpers::Runners::makeRunnerMock($runner);
    my $env = $runner->getEnvironment();
    is($env->getName(), 'mono');
}

run_EnvironmentIsSpecified_UseSpecifiedEnvironment:
{
    my $prepareEnvInvoked = 0;
    my $env = new Environments::MonoBleedingEdge();
    my $config = {
        name => 'suite_name',
        nunitInputFiles => ['inputfile1.dll'],
        environment => $env
    };

    my $runner = makeRunnerMock($config);
    $runner = TestHelpers::Runners::makeRunnerMock($runner);
    $env = $runner->getEnvironment();
    is($env->getName(), 'monobe');
}

run_AssetPipelineV2IsDefined_SetsEnvironmentVariable:
{
    my $envStateDuringRun;
    my $config = {
        name => 'suite_name',
        nunitInputFiles => ['inputfile1.dll']
    };
    my $runner = makeRunnerMock($config);
    my @args = ('--assetpipelinev2');
    $runner = TestHelpers::Runners::makeRunnerMock($runner, @args);
    $runner->mock(
        'doRun' => sub
        {
            my ($this) = @_;
            $envStateDuringRun = $ENV{'UNITY_USE_ASSET_PIPELINE_V2'};
            $this->{runnerExitCode} = 0;
        },
    );

    $ENV{'UNITY_USE_ASSET_PIPELINE_V2'} = -1;
    $runner->run(@args);

    is($envStateDuringRun, 1);
    isnt($ENV{'UNITY_USE_ASSET_PIPELINE_V2'}, 1);
}

run_AssetPipelineV2IsNotDefined_ResetsEnvironmentVariable:
{
    my $envStateDuringRun;
    my $config = {
        name => 'suite_name',
        nunitInputFiles => ['inputfile1.dll']
    };
    my $runner = makeRunnerMock($config);
    $runner = TestHelpers::Runners::makeRunnerMock($runner);
    $runner->mock(
        'doRun' => sub
        {
            my ($this) = @_;
            $envStateDuringRun = $ENV{'UNITY_USE_ASSET_PIPELINE_V2'};
            $this->{runnerExitCode} = 0;
        },
    );

    $ENV{'UNITY_USE_ASSET_PIPELINE_V2'} = 1;
    $runner->run();

    isnt($envStateDuringRun, 1);
    isnt($ENV{'UNITY_USE_ASSET_PIPELINE_V2'}, 1);
}

run_PrivateBinPath_BuildsTestListOnlyFromThoseProjects:
{
    my $solutionPath = canonpath("$utrRoot/t/TestData/VSSolution/VSSolution.sln");
    my $config = {
        name => 'suite_name',
        nunitInputFiles => ['inputfile1.dll']
    };

    my $runner = makeRunnerMock($config);

    $runner = TestHelpers::Runners::makeRunnerMock($runner);

    my @grabbedProjects;
    $runner->mock(
        'buildCSProject' => sub
        {
            my (undef, $proj) = @_;
            push(@grabbedProjects, $proj);
        }
    );

    my @input = runAndGrabExecuteInput($runner, '--privatebinpath=path1', '--privatebinpath=path2');
    arrayContains(\@input, ['--privatebinpath=path1;path2']);
}

done_testing();

sub makeRunnerMock
{
    my ($config) = @_;
    my $runner = new Test::MockObject::Extends(new NUnitConfigBasedSuiteRunner());
    my $nameBuilderStub = makeDefaultSuiteRunnerArtifactNameBuilderStub();
    $runner->mock(
        getConfig => sub
        {
            return $config;
        }
    );
    $runner->setArtifactsNameBuilder($nameBuilderStub->object());
    return $runner;
}
