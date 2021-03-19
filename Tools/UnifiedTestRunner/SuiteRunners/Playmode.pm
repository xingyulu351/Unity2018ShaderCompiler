package SuiteRunners::Playmode;

use parent SuiteRunner;

use Dirs;

use warnings;
use strict;
use File::Spec::Functions qw (canonpath catfile catdir);
use File::Path qw (mkpath);

my $root = Dirs::getRoot();
my $playmodeTestRunner = canonpath("$root/Tests/Unity.PlaymodeTestsRunner/Unity.PlaymodeTestsRunner.csproj");
my $unifiedTestRunnerCSharpProj = canonpath("$root/Tests/Unity.UnifiedTestRunner/Unity.UnifiedTestRunner.csproj");

my $suiteOptions = [
    ['build-mode:s', 'Build mode', { hidden => 1, defaultValue => 'minimal' }],
    ['build-isolation:s', undef, { hidden => 1, defaultValue => 'enabled' }],
    [
        'autodoc-file:s',
        'Path to autodocs option file',
        {
            hidden => 1,
            defaultValue => "$root/Tests/Docs/docs/TestFrameworks/UnityTestFramework/EditorTests.UnifiedTestRunner.Options.generated.md",
        }
    ],
    ['update-runner-options', 'Updates options json file', { hidden => 1 }],
    ['disable-errors-detection', 'Updates options json file', { hidden => 1, defaultValue => 1 }],
    ['open-report', 'Open HTML report in browser if test execution failed'],
    ['zero-tests-are-ok:n', 'Consider suite with 0 test in it as a failure', { hidden => 1, defaultValue => 1 }],
];

sub new
{
    my ($pkg) = @_;
    my $this = $pkg->SUPER::new(
        suiteOptions => $suiteOptions,
        optionsGenProj => $unifiedTestRunnerCSharpProj,
        testRunnerLibProj => $playmodeTestRunner,
    );
    $this->{testresultsxml} = [];
    return $this;
}

sub doRun
{
    my ($this) = @_;
    $this->prepareBuildTarget($this->getPlatform());
    my $projectList = $this->getOptionValue('projectlist');
    my $humanLog = $this->getHumanLogPath();
    my $machineLong = $this->getMachineLogPath();

    my @args = ("--suite=" . $this->getName(), "--artifacts_path=" . $this->getArtifactsPath());

    if ($this->getOptionValue('open-report'))
    {
        push(@args, '--open-report');
    }

    $this->_addRunnerOptions(\@args);

    $this->{runnerExitCode} = $this->runDotNetProgram(
        program => $this->getPathToTestRunner(),
        programArgs => \@args,
        filesToWatch => [
            { file => $machineLong, callback => \&SuiteRunner::watchTestProtocolMessages, callbackArg => $this },
            { file => $humanLog, callback => \&Plugin::printLogMessage, callbackArg => $this },
        ]
    );
}

sub startNewIteration
{
    my ($this) = @_;
    $this->{artifact_folders_prepared} = 0;
}

sub beforeRun
{
    my ($this) = @_;

    # Using this to override player settings when running multiple suites with UTR.
    # Alternative would be to always have correct settings in every playmode test project.
    $ENV{'UNITY_PLAYMODEWITHUTR'} = '1';

    #playmode and editmode are not doing any prepare
    $this->prepareBuildTarget($this->getOptionValue('platform'));
}

sub prepareBuildTarget
{
    my ($this, $platform) = @_;
    $this->SUPER::prepareBuildTarget($platform);

    if (lc $platform eq 'iOS' || lc $platform eq 'tvOS')
    {
        $ENV{'UNITY_PATHTOIOSDEPLOY'} = "$root/PlatformDependent/iPhonePlayer/External/ios-deploy/1.9.2/ios-deploy";
    }
}

sub buildDependencies
{
    my ($this) = @_;
    $this->buildCSDependencies($playmodeTestRunner, $unifiedTestRunnerCSharpProj);
    $this->{dependencies_built} = 1;
}

sub afterRun
{
    my ($this) = @_;

    my $platform = $this->getOptionValue('platform');
    if (lc $platform eq 'android')
    {
        $this->SUPER::disposeBuildTarget($platform);
    }
}

sub usage
{
    my ($this) = @_;
    return (
        name => $this->getName(),
        description => 'Playmode tests.',
        url => 'https://confluence.hq.unity3d.com/display/DEV/Playmode+Tests',
        examples => [{ description => 'Run ABV playmode tests', command => '--category=ABV' }]
    );
}

sub getName
{
    return 'playmode';
}

sub getPlatform
{
    my ($this) = @_;
    return $this->getName();
}

sub getPathToTestRunner
{
    my ($this) = @_;
    return $this->_getAssemblyFullPath($unifiedTestRunnerCSharpProj);
}

sub getHumanLogPath
{
    my ($this) = @_;
    return catfile($this->getArtifactsPath(), "TestRunnerLog.txt");
}

sub getMachineLogPath
{
    my ($this) = @_;
    return catfile($this->getArtifactsPath(), "TestRunnerLog.json");
}

sub registerStaticArtifacts
{

}

sub prepareArtifactsFolders
{
    my ($this) = @_;
    if ($this->{artifact_folders_prepared})
    {
        return;
    }
    Logger::minimal("preparing artifacts folder");

    my $artifactsPath = $this->getArtifactsPath();
    if (not -d $artifactsPath)
    {
        my $res = mkpath($artifactsPath);
        if (not $res)
        {
            croak("Can not create dir: $artifactsPath");
        }
        $this->createArtifactsDirIfNotExistsOrCroak("UnifiedTestRunner");
    }
}

sub processResults
{
    my ($this) = @_;
    my $jsonReportPath = catfile($this->getArtifactsPath(), 'TestReportData.json');
    my $testReport = FileUtils::readJsonObj($jsonReportPath);
    foreach my $suite (@{ $testReport->{suites} })
    {
        my $suiteResult = SuiteResult::fromJson($suite);
        $suiteResult->setZeroTestsAreOk($this->getOptionValue('zero-tests-are-ok'));
        $this->writeSuiteData($suiteResult);
        $this->sendResultToConsumer($suiteResult);
    }
}

1;
