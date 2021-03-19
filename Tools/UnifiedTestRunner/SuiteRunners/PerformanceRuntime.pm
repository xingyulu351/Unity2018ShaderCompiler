package SuiteRunners::PerformanceRuntime;

use parent RuntimeBase;

use File::Spec::Functions qw (canonpath catfile);

use warnings;
use strict;

use Dirs;
use LineTail;
use TestPlan;
use threads::shared;

use CSProj;
use FileUtils;
use PerformanceDataPostProcessor;
use Dirs;
use lib Dirs::external_root();
use JSON;
use RunnerOptions;
use Test::MockModule;
use FileUtils;

my $defaultProjPath = "Unity.PerformanceRuntimeTests/Unity.PerformanceRuntimeTests.csproj";

my $root = Dirs::getRoot();
my $runnerProj = canonpath("$root/Tests/Unity.PerformanceTests.RuntimeTestRunner/Unity.PerformanceTests.RuntimeTestRunner.csproj");
my $reporterProj = canonpath("$root/Tests/Unity.PerformanceTests.Reporter/Unity.PerformanceTests.Reporter.csproj");

my $suiteOptions = [
    [
        'platform=s',
        'Specifies the platform to run the tests on.',
        {
            allowedValues => ['standalone', 'ios', 'android', 'iphone', 'editor'],
            defaultValue => 'editor'
        }
    ],
    [
        'testproject=s',
        'Name of test CS project with tests',
        {
            allowedValues => [
                'Unity.PerformanceRuntimeTests/Unity.PerformanceRuntimeTests.csproj',
                'Unity.PerformanceRuntimeTests.Xr/Unity.PerformanceRuntimeTests.Xr.csproj'
            ],
            defaultValue => 'Unity.PerformanceRuntimeTests/Unity.PerformanceRuntimeTests.csproj'
        }
    ],
    ['build-mode:s', 'Build mode', { hidden => 1, defaultValue => 'full' }],
    ['build-isolation:s', undef, { hidden => 1, defaultValue => 'none' }],
    {
        prototypes => ['testresultsxml:s', 'testresultsjson:s', 'testlist:s']
    },
];

sub new
{
    my ($pkg) = @_;
    my @reporterOptions = RunnerOptions::readFromFile(_getReporterOptionsFileName());
    return $pkg->SUPER::new(
        suiteOptions => $suiteOptions,
        additionalOptions => \@reporterOptions,
        optionsGenProj => $runnerProj
    );
}

sub usage
{
    my ($this) = @_;
    return (
        description => "Performance runtime tests",
        prerequisites => "perl build.pl repos fetch Tests/PerformanceTestAssets",
        url => 'http://confluence.hq.unity3d.com/display/DEV/Performance+Runtime+Tests',
        examples => [{ description => "Run tests matching 'Animators' 10 times", command => '--executioncount=10 --testfilter=Animator' },],
    );
}

sub getName
{
    my ($this) = @_;
    return 'performance-runtime';
}

sub getReporterSuiteName
{
    my ($this) = @_;
    return 'Runtime';
}

sub getPlatform
{
    my ($this) = @_;
    return $this->getOptionValue('platform');
}

sub getTestProject
{
    my ($this) = @_;
    my @subPath = $this->getOptionValue('testproject');
    return canonpath("$root/Tests/$subPath[0]");
}

sub buildDependencies
{
    my ($this) = @_;
    if ($this->{dependencies_built})
    {
        return;
    }
    $this->cleanCSharpProjectsSln();
    $this->buildCSProject($runnerProj);
    $this->buildCSProject($reporterProj);
    $this->buildCSProject($this->getTestProject());

    $this->{dependencies_built} = 1;
}

sub beforeRun
{
    my ($this) = @_;
    $this->writeTestPlan($this->testPlanFileName(), $this->getTestPlan());
}

sub registerStaticArtifacts
{
    my ($this) = @_;
    my $stdOut = $this->stdOutFileName();
    my $stdErr = $this->stdErrFileName();
    $this->registerArtifacts($stdOut, $stdErr);
}

sub doRun
{
    my ($this) = @_;
    $this->prepareBuildTarget($this->getPlatform());
    my @args = $this->prepareArgs();
    my $stdOut = $this->stdOutFileName();
    my $stdErr = $this->stdErrFileName();

    $this->{runnerExitCode} = $this->runDotNetProgram(
        program => $this->getPathToTestRunner(),
        programArgs => \@args,
        filesToWatch => [
            { file => $stdOut, callback => \&RuntimeBase::printProgressMessage, callbackArg => $this },
            { file => $stdOut, callback => \&Plugin::printLogMessage, callbackArg => $this },
            { file => $stdErr, callback => \&Plugin::printLogErrorMessage, callbackArg => $this }
        ]
    );

    # Dont report performance data in QV until all tests are stable enough
    my $category = $this->getOptionValue('category');
    if (defined($category) and $category =~ /^QV$/)
    {
        return;
    }
    my $reporterStdOut = $this->reporterStdOutFileName();
    my $reporterStdErr = $this->reporterStdErrFileName();
    $this->registerArtifacts($reporterStdOut, $reporterStdErr);
    $this->_reportPerformanceData();
}

sub getPathToTestRunner
{
    my ($this) = @_;
    return $this->_getAssemblyFullPath($runnerProj);
}

sub getPathToReporter
{
    my ($this) = @_;
    return $this->_getAssemblyFullPath($reporterProj);
}

sub prepareArgsForListTests
{
    my ($this) = @_;
    my @args = ();

    push(@args, "--list");

    my $platform = $this->getOptionValue('platform');
    push(@args, "--platform=$platform");

    my $area = $this->getOptionValue('area');
    if (defined($area))
    {
        push(@args, "--area=$area");
    }

    my $filter = $this->getOptionValue('testfilter');
    if (defined($filter))
    {
        push(@args, "--testfilter=$filter");
    }

    my $testAssembly = $this->_getTestAssemblyFullPath();
    push(@args, "--assemblywithtests=$testAssembly");

    $this->_addRunnerOptions(\@args);

    my $testPlanFileName = $this->testPlanFileName();
    push(@args, "1>$testPlanFileName");

    return @args;
}

sub prepareArgs
{
    my ($this) = @_;

    my @args = ();

    my $platform = $this->getPlatform();
    push(@args, "--platform=$platform");

    my $area = $this->getOptionValue('area');
    if (defined($area))
    {
        push(@args, "--area=$area");
    }

    my $filter = $this->getOptionValue('testfilter');
    if (defined($filter))
    {
        push(@args, "--testfilter=$filter");
    }

    my $testlist = $this->getOptionValue('testlist');
    if (defined($testlist))
    {
        push(@args, "--testlist=$testlist");
    }

    my $testresultsxml = $this->testResultXmlFileName($this->getOptionValue('testresultsxml'));
    if (defined($testresultsxml))
    {
        push(@args, "--testresultsxml=$testresultsxml");
    }

    my $testAssembly = $this->_getTestAssemblyFullPath();
    my $assemblyDir = FileUtils::getDirName($testAssembly);
    push(@args, "--assemblywithtests=$testAssembly");
    push(@args, "--extensionssearchpaths=$assemblyDir");
    push(@args, "--artifactspath=" . $this->getArtifactsPath());

    $this->_addRunnerOptions(\@args);

    my $stdOut = $this->stdOutFileName();
    push(@args, "1>$stdOut");

    my $stdErr = $this->stdErrFileName();
    push(@args, "2>$stdErr");

    return @args;
}

sub prepareArtifactsFolders
{
    my ($this) = @_;
    if ($this->{artifact_folders_prepared})
    {
        return;
    }
    $this->createArtifactsDirIfNotExistsOrCroak('Unity.PerformanceTests.RuntimeTestRunner');
    $this->createArtifactsDirIfNotExistsOrCroak('Unity.PerformanceTests.Reporter');
    $this->createArtifactsDirIfNotExistsOrCroak('UnifiedTestRunner');
    $this->{artifact_folders_prepared} = 1;
}

sub _getTestAssemblyFullPath
{
    my ($this) = @_;
    my $proj = $this->getTestProject();
    my $csproj = new CSProj($proj);
    my $dllPath = $csproj->getAssemblyFullPath();
    return $dllPath;
}

sub writeSuiteData
{
    my ($this, $suiteResult) = @_;
    my $performanceDataFile = $this->performanceDataFileName();
    if (-e $performanceDataFile)
    {
        my $json = join(q (), FileUtils::readAllLines($performanceDataFile));
        PerformanceDataPostProcessor::embedPerformanceData(jsonToObj($json), $suiteResult);
    }
}

sub updateRunnerHelp
{
    my ($this) = @_;
    if ($this->{helpUpdated})
    {
        return (undef, 0);
    }
    my (undef, $exitCode) = $this->SUPER::updateRunnerHelp();
    if ($exitCode == 0)
    {
        my $reporterOptionsFileName = _getReporterOptionsFileName();
        return $this->_doUpdateRunnerHelp($reporterProj, $reporterOptionsFileName);
    }
}

sub _getReporterOptionsFileName
{
    return canonpath(catfile(Dirs::UTR_root(), 'Configs', "Unity.PerformanceTests.Reporter.Options.json"));
}

sub stdOutFileName
{
    return performanceRuntimeTestRunnerStdOutFileName(@_);
}

sub stdErrFileName
{
    return performanceRuntimeTestRunnerStdErrFileName(@_);
}

sub reporterStdOutFileName
{
    return performanceReporterStdOutFileName(@_);
}

sub reporterStdErrFileName
{
    return performanceReporterStdErrFileName(@_);
}

sub testResultXmlFileName
{
    return performanceRuntimeTestRunnerTestResultXmlFile(@_);
}

sub performanceRuntimeTestRunnerTestResultXmlFile
{
    my ($this) = @_;
    return $this->testResultXmlFileNameImpl('Unity.PerformanceTests.RuntimeTestRunner');
}

sub performanceRuntimeTestRunnerStdOutFileName
{
    my ($this) = @_;
    return $this->{artifactsNameBuilder}->getFullAtrifactFileName(
        'parent-type' => 'Unity.PerformanceTests.RuntimeTestRunner',
        type => 'stdout',
        extension => 'txt'
    );
}

sub performanceRuntimeTestRunnerStdErrFileName
{
    my ($this) = @_;
    return $this->{artifactsNameBuilder}->getFullAtrifactFileName(
        'parent-type' => 'Unity.PerformanceTests.RuntimeTestRunner',
        type => 'stderr',
        extension => 'txt'
    );
}

sub performanceReporterStdOutFileName
{
    my ($this) = @_;
    return $this->{artifactsNameBuilder}->getFullAtrifactFileName(
        'parent-type' => 'Unity.PerformanceTests.Reporter',
        type => 'stdout',
        extension => 'txt'
    );
}

sub performanceReporterStdErrFileName
{
    my ($this) = @_;
    return $this->{artifactsNameBuilder}->getFullAtrifactFileName(
        'parent-type' => 'Unity.PerformanceTests.Reporter',
        type => 'stderr',
        extension => 'txt'
    );
}

sub performanceDataFileName
{
    my ($this) = @_;
    my ($path) = $this->getOptionValue('outputjson');
    if ($path)
    {
        return $path;
    }
    return $this->{artifactsNameBuilder}->getFullAtrifactFileName(
        'parent-type' => 'Unity.PerformanceTests.Reporter',
        type => 'perfdata',
        extension => 'json'
    );
}

1;
