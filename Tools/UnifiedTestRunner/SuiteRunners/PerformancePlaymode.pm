package SuiteRunners::PerformancePlaymode;

use parent UnityTestFrameworkSuiteRunnerBase;

use warnings;
use strict;

use Switch;

use File::Spec::Functions qw (canonpath splitdir catfile);

use Dirs;
use lib Dirs::external_root();
use JSON;

use constant {
    RETURN_CODE_TESTS_OK => 0,
    RETURN_CODE_TESTS_FAILED => 2,
    RETURN_CODE_NO_TESTS_FOUND => 3,
    RETURN_CODE_FAILED_TO_RESOLVE_PROJECT_PATH => 4
};

my $root = Dirs::getRoot();
my $reporterProj = canonpath("$root/Tests/Unity.PerformanceTests.Reporter/Unity.PerformanceTests.Reporter.csproj");

my $projectListFileName = 'Tests/PerformanceTests/projectlist.txt';

my $projectName;

my $suiteOptions = [
    ['testprojects:s@', "Name(s) of the test project from the $projectListFileName"],
    [
        'projectlist:s',
        'File contains list of projects with playmode tests',
        {
            defaultValue => $projectListFileName
        }
    ],
    ['categories:s@', "Filter tests by categories e.g. 'ABV'"],
    [
        'platform:s',
        'Platform to run the tests on',
        {
            defaultValue => 'playmode',
            allowedValues => [
                'playmode', 'StandaloneLinuxUniversal', 'StandaloneOSXIntel', 'StandaloneOSXUniversal',
                'StandaloneWindows', 'StandaloneWindows64', 'PS4', 'Android'
            ]
        }
    ],
    ['assetpipelinev2', 'Enable Asset Import Pipeline V2 backend for the test run.']
];

sub new
{
    my ($pkg) = @_;
    my @reporterOptions = RunnerOptions::readFromFile(_getReporterOptionsFileName());
    my $this = $pkg->SUPER::new(
        suiteOptions => $suiteOptions,
        additionalOptions => \@reporterOptions
    );
    $this->{testresultsxml} = [];
    return $this;
}

sub beforeRun
{
    my ($this) = @_;

    #playmode and editmode are not doing any prepare
    $this->prepareBuildTarget($this->getOptionValue('platform'));
}

sub usage
{
    my ($this) = @_;
    return (
        name => $this->getName(),
        description => 'Performance playmode tests.',
        url => 'http://internaldocs.hq.unity3d.com/automation/TestFrameworks/PerformanceTests/PlaymodeAndEditmode/index.html',
        examples => [{ description => 'Run QV playmode tests', command => '--categories=QV' }]
    );
}

sub getName
{
    return 'performance-playmode';
}

sub getReporterSuiteName
{
    return 'Playmode';
}

sub getPlatform
{
    my ($this) = @_;
    return $this->getOptionValue('platform');
}

sub buildDependencies
{
    my ($this) = @_;
    if ($this->{dependencies_built})
    {
        return;
    }
    $this->cleanCSharpProjectsSln();
    $this->buildCSProject($reporterProj);
    $this->{dependencies_built} = 1;
}

sub doRun
{
    my ($this) = @_;
    my @projectPaths = $this->resolveProjects();
    if (scalar(@projectPaths) == 0)
    {
        $this->_processProjectNotRevoledError();
        return;
    }

    $this->{runnerExitCode} = RETURN_CODE_TESTS_OK;
    foreach my $projectPath (@projectPaths)
    {
        my $projectExitCode = $this->doRunUnityProject($projectPath);
        $projectName = getLastFolder($projectPath);

        switch ($projectExitCode)
        {
            case RETURN_CODE_TESTS_OK { Logger::minimal("Completed tests for project $projectName ExitCode:$projectExitCode"); }
            case RETURN_CODE_TESTS_FAILED { Logger::minimal("Failed tests for project $projectName ExitCode:$projectExitCode"); }
            case RETURN_CODE_NO_TESTS_FOUND { Logger::warning("No tests for project $projectName"); }
            else
            {
                Logger::error("Unknow exitcode $projectExitCode while running $projectName");
            }
        }
        my $reporterStdOut = $this->reporterStdOutFileName($projectName);
        my $reporterStdErr = $this->reporterStdErrFileName($projectName);
        $this->registerArtifacts($reporterStdOut, $reporterStdErr);
        $this->_reportPerformanceData($projectName);
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

sub getLastFolder
{
    my ($dirPath) = @_;
    my (@dirs) = splitdir($dirPath);
    return $dirs[-1];
}

sub getPathToReporter
{
    my ($this) = @_;
    return $this->_getAssemblyFullPath($reporterProj);
}

sub reporterStdOutFileName
{
    my ($this) = @_;
    return $this->{artifactsNameBuilder}->getFullAtrifactFileName(
        'parent-type' => "$projectName/Unity.PerformanceTests.Reporter",
        type => 'stdout',
        extension => 'txt'
    );
}

sub reporterStdErrFileName
{
    my ($this) = @_;
    return $this->{artifactsNameBuilder}->getFullAtrifactFileName(
        'parent-type' => "$projectName/Unity.PerformanceTests.Reporter",
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
        'parent-type' => "$projectName/Unity.PerformanceTests.Reporter",
        type => 'perfdata',
        extension => 'json'
    );
}

sub writeSuiteData
{
    my ($this, $suiteResult, $projectName) = @_;
    my $artifactsPath = $this->getArtifactsPath();
    my $jsonReportFileName = catfile($artifactsPath, $projectName, 'Unity.PerformanceTests.Reporter', 'perfdata.json');
    if (-e $jsonReportFileName)
    {
        my $json = join(q (), FileUtils::readAllLines($jsonReportFileName));
        PerformanceDataPostProcessor::embedPerformanceData(jsonToObj($json), $suiteResult);
    }
}

sub getExtraArguments
{
    my ($this, $projectName, @cmdArgs) = @_;
    my $testresultsjson = $this->testResultJsonFileName($projectName);
    return ("-performanceTestResults $testresultsjson");
}

sub testResultJsonFileName
{
    my ($this, $projectName) = @_;
    my $parentType = $this->getParentType($projectName);
    return $this->{artifactsNameBuilder}->getFullAtrifactFileName(
        'parent-type' => $parentType,
        type => 'TestResults',
        extension => 'json'
    );
}

1;
