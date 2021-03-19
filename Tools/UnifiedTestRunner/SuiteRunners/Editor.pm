package SuiteRunners::Editor;

use parent SuiteRunner;

use warnings;
use strict;
use Dirs;

use File::Spec::Functions qw (canonpath catfile catdir);
use File::Path qw (mkpath);

my $root = Dirs::getRoot();
my $editorTestRunner = canonpath("$root/Tests/Unity.EditorTestsRunner/Unity.EditorTestsRunner.csproj");
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
    ['open-report', 'Open HTML report in browser if test execution failed'],
    ['zero-tests-are-ok:n', 'Consider suite with 0 test in it as a failure', { hidden => 1, defaultValue => 1 }],
];

sub new
{
    my ($pkg) = @_;
    my $this = $pkg->SUPER::new(
        suiteOptions => $suiteOptions,
        optionsGenProj => $unifiedTestRunnerCSharpProj,
        testRunnerLibProj => $editorTestRunner,
    );
}

sub usage
{
    my ($this) = @_;
    return (
        description => 'Editor tests.',
        url => 'https://confluence.hq.unity3d.com/display/DEV/Editor+Tests',
        examples => [
            {
                description => "Run test from 'Undo' and 'Animations' projects only",
                command => '--testprojects=Undo  --testprojects=Animation'
            },
            { description => 'Run ABV editor tests', command => '--category=ABV' },
            { description => 'Run 3D asset import tests', command => '--projectlist=Tests/3DAssetImportTest/projectlist.txt' }
        ],
    );
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

    $this->runDotNetProgram(
        program => $this->getPathToTestRunner(),
        programArgs => \@args,
        filesToWatch => [
            { file => $machineLong, callback => \&SuiteRunner::watchTestProtocolMessages, callbackArg => $this },
            { file => $humanLog, callback => \&Plugin::printLogMessage, callbackArg => $this },
        ]
    );
}

sub getName
{
    return "editor";
}

sub getPlatform
{
    return "editor";
}

sub buildDependencies
{
    my ($this) = @_;
    $this->buildCSDependencies($editorTestRunner, $unifiedTestRunnerCSharpProj);
    $this->{dependencies_built} = 1;
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

sub _getTestRunnerOptionsFileName
{
    return canonpath(catfile(Dirs::UTR_root(), 'Configs', "Unity.EditorTestRunner.Options.json"));
}

sub onMessage
{
    # do notthing
}

sub startNewIteration
{
    my ($this) = @_;
    $this->{artifact_folders_prepared} = 0;
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

1;
