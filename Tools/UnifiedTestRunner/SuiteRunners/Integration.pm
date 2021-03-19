package SuiteRunners::Integration;

use parent SuiteRunner;

use warnings;
use strict;

use Dirs;
use lib Dirs::external_root();
use TestResult;
use IntegrationTestTarget;
use Carp qw (croak);
use Cwd qw (abs_path realpath);
use File::Basename qw (dirname);
use File::Spec::Functions qw (canonpath catfile);

use TargetAttributesResolver;
use CSProj;
use VSSolution;
use SuiteRunnerArtifactsNameBuilder;
use FileWatcher;
use ArtifactsMapBuilder;
use Readonly;
use FileUtils qw (openForReadOrCroak closeOrCroak openForWriteOrCroak readJsonObj);
use Option;
use MSBuildProjectGenerator;
use List::MoreUtils qw (uniq);
use Hoarder::Collectors::ProductVersion;
use ParallelTestsSelector;
use JSON;
use PerformanceDataPostProcessor;

my $root = Dirs::getRoot();

my $intergrationTestRunner = canonpath("$root/Tests/Unity.IntegrationTestsRunner/Unity.IntegrationTestsRunner.csproj");
my $unifiedTestRunnerCSharpProj = canonpath("$root/Tests/Unity.UnifiedTestRunner/Unity.UnifiedTestRunner.csproj");
my $reporterProj = canonpath("$root/Tests/Unity.PerformanceTests.Reporter/Unity.PerformanceTests.Reporter.csproj");

my @allCSProjects = (
    'Unity.IntegrationTests.gen.csproj',
    'Unity.PureCSharpTests.csproj',
    ,
    'Unity.IntegrationTests.Framework.gen.csproj',
    'Unity.IntegrationTests.Debugger.csproj',
    'Unity.CommonTools.Tests.csproj',
    'Unity.Automation.Tests.csproj',
);

my $testTargetDefaultValue = 'TestAll';

my $suiteOptions = [
    [
        'testtarget:s',
        'MSBuild (xbuild) target from Projects\CSharp\TeamCityTests.proj. Multiple targets can be specifed',
        {
            defaultValue => $testTargetDefaultValue,
            allowedValues => [_collectTargets()]
        }
    ],
    ['testlist:s', 'File contains list of tests. Each on the new line'],
    [
        'testprojects:s@',
        'Name(s) of test CS projects with tests',
        {
            defaultValue => ['all'],
            allowedValues => \@allCSProjects,
        }
    ],
    ['unity-artifacts-folder:s', 'Path to store various unity artifacts such as editor build log, player log, etc.', { hidden => 1 }],
    ['forceusbconnection', 'Force Android tests to use ADB over USB instead of network connection'],

    #TODO overridie build  default value until it become, global default
    ['build-mode:s', 'Build mode', { hidden => 1, defaultValue => 'minimal' }],
    ['build-isolation:s', undef, { hidden => 1, defaultValue => 'enabled' }],
    ['smart-select', 'Enables smart tests selection based on hoader statistics data', { defaultValue => 0, hidden => 1 }],
    [
        'smart-tests-selector-uri:s',
        'Smart tests selector web service uri',
        { defaultValue => 'http://api.hoarder.qa.hq.unity3d.com/v1/SmartTestsSelector', hidden => 1 }
    ],
    [
        'autodoc-file:s',
        'Path to autodocs option file',
        {
            hidden => 1,
            defaultValue => "$root/Tests/Docs/docs/TestFrameworks/IntegrationTests/UnifiedTestRunner.Options.generated.md"
        }
    ],
    ['assetpipelinev2', 'Enable Asset Import Pipeline V2 backend for the test run.'],
    ['open-report', 'Open HTML report in browser if test execution failed'],
];

sub new
{
    my ($pkg) = @_;
    my $this = $pkg->SUPER::new(suiteOptions => $suiteOptions);
    $this->{artifact_folders_prepared} = 0;
    $this->{dependencies_built} = 0;
    return $this;
}

sub _getAllCSProjects
{
    my ($this) = @_;
    return @allCSProjects;
}

sub usage
{
    my ($this) = @_;
    return (
        description => "NUnit tests spread among different CS projects. Also known as 'integration' tests.",
        url => 'http://internaldocs.hq.unity3d.com/automation/TestFrameworks/IntegrationTests/About/index.html',
        prerequisites =>
            "jam ManagedProjectFiles' is sufficient to run all type of 'pure' C# tests, e.g. -'-testprojects=Unity.PureCSharpTests'. For other tests, you should build corresponding executable. E.g. standalone player for '--testtarget=TestStandalone'",
        examples => [
            {
                description => "Run VeryVerySlow tests from only from the Unity.IntegrationTests.csproj",
                command => '--testtarget=TestVeryVerySlowNoInstallEditor --testfilter=UNET'
            },
            {
                description => "Run tests from Unity.PureCSharpTests.csproj and Mono.Options.Tests.csproj",
                command => '--testprojects=Unity.PureCSharpTests.csproj --testprojects=Mono.Options.Tests.csproj'
            },
            {
                description => "Run VeryVerySlow tests matching 'UNET' from all the C# projects",
                command => '--testtarget=TestVeryVerySlowNoInstallEditor --testfilter=UNET'
            },
            { description => "List tests for default test target, matching UNET", command => '--list --testfilter=UNET' }
        ]
    );
}

sub getName
{
    my ($this) = @_;
    return 'integration';
}

sub getReporterSuiteName
{
    my ($this) = @_;
    return 'Integration';
}

sub getPlatform
{
    my ($this) = @_;
    return $this->getTestTarget();
}

sub getTestTarget
{
    my ($this) = @_;
    return $this->getOptionValue('testtarget');
}

sub registerStaticArtifacts
{
    my ($this) = @_;
    $this->registerArtifacts($this->getHumanLogPath());
}

sub buildDependencies
{
    my ($this) = @_;
    if ($this->{dependencies_built})
    {
        return;
    }
    $this->cleanCSharpProjectsSln();
    my @testProjectPaths = $this->resolveTestProjects();
    my $isFilterUsed = $this->getOptionValue('testfilter');
    my $projectsCount = scalar(@testProjectPaths);

    if ($isFilterUsed and $projectsCount > 1)
    {
        my $msg = "Specify the 'testproject' to reduce compilation time E.g.:\n";
        $msg .= "\t--testprojects=IntegrationTests.gen.csproj\n";
        $msg .= "\tSee " . Commands::getUtrPrefix() . " --suite=integration --help.";
        Logger::warning($msg);
    }

    push(@testProjectPaths, $unifiedTestRunnerCSharpProj, $intergrationTestRunner);
    $this->buildCSDependencies(@testProjectPaths);
    $this->{dependencies_built} = 1;
}

sub prepareArtifactsFolders
{
    my ($this) = @_;
    $this->createArtifactsDirIfNotExistsOrCroak('UnifiedTestRunner');
}

sub beforeRun
{
    my ($this) = @_;
    $this->prepareEnvironmentVariables();
}

sub afterRun
{
    my ($this) = @_;
    $this->resetEnvironmentVariables();
}

sub doRun
{
    my ($this) = @_;
    $this->prepareBuildTargets();
    my @dlls = $this->getAssemblyWithTests();

    my $testPlan = $this->getTestPlan();
    my $testPlanFileName = catfile($this->getArtifactsPath(), "TestList.txt");

    FileUtils::writeAllLines($testPlanFileName, $testPlan->getTestNames());

    my $humanLog = $this->getHumanLogPath();
    my $machineLog = $this->getMachineLogPath();
    my $testRunner = $this->_getAssemblyFullPath($unifiedTestRunnerCSharpProj);
    my $filesToWatch = [
        { file => $humanLog, callback => \&Plugin::printLogMessage, callbackArg => $this },
        { file => $machineLog, callback => \&SuiteRunner::watchTestProtocolMessages, callbackArg => $this },
    ];

    my @args;
    push(@args, "--suite=integration");
    push(@args, "--artifacts_path=" . $this->getArtifactsPath());
    push(@args, "--testlist=" . $testPlanFileName);
    if ($this->getOptionValue('open-report'))
    {
        push(@args, '--open-report');
    }
    $this->_addRunnerOptions(\@args);

    push(@args, @dlls);
    $this->{runnerExitCode} = $this->runDotNetProgram(
        program => $testRunner,
        programArgs => \@args,
        filesToWatch => $filesToWatch,
    );
}

sub queryPartitions
{
    my ($this) = @_;
    my %repositoryInfo = RepositoryInfo::get();
    $this->prepareArtifactsFolders();
    $this->buildDependencies();
    return ParallelTestsSelector::queryPartitions(
        %repositoryInfo,
        uri => $this->getOptionValue('parallel-tests-selector-uri'),
        configId => $this->getOptionValue('config-id'),
        submitter => $this->getOptionValue('submitter'),
        testPlan => $this->getTestPlan(),
        partitionCount => $this->getOptionValue('partition-count'),
        filterUsingSmartSelect => $this->getOptionValue('smart-select'),
        timeout => $this->getOptionValue('query-partitions-timeout'),
        retries => $this->getOptionValue('query-partitions-retries'),
    );
}

sub prepareEnvironmentVariables
{
    my ($this) = @_;
    ## no critic (RequireLocalizedPunctuationVars)
    $ENV{'UNITY_UNIFIED_TEST_RUNNER_UNITY_ARTIFACTS_FOLDER'} = $this->getUnityArtifactsFolder();

    my $forceUsbConnection = $this->getOptionValue('forceusbconnection');
    if ($forceUsbConnection)
    {
        $ENV{'UNITY_FORCE_ANDROID_USB_CONNECTION'} = 1;
    }
    if ($this->getOptionValue('assetpipelinev2'))
    {
        $ENV{'UNITY_USE_ASSET_PIPELINE_V2'} = 1;
    }
}

sub resetEnvironmentVariables
{
    my ($this) = @_;
    delete $ENV{'UNITY_UNIFIED_TEST_RUNNER_UNITY_ARTIFACTS_FOLDER'};
    delete $ENV{'UNITY_USE_ASSET_PIPELINE_V2'};
    delete $ENV{'UNITY_FORCE_ANDROID_USB_CONNECTION'};
}

sub getRuntimePlatforms
{
    my ($this) = @_;
    my $target = $this->resolveTarget();
    return $target->getRuntimePlatforms();
}

sub resolveTarget
{
    my ($this) = @_;
    if ($this->{target})
    {
        return $this->{target};
    }
    my $root = Dirs::getRoot();
    my $teamcityProjectPath = canonpath("$root/Projects/CSharp/TeamCityTests.proj");
    my $resolver = new TargetAttributesResolver($teamcityProjectPath);
    my $targetName = $this->getTestTarget();
    my $target = $resolver->resolve($targetName);
    if (not defined($target))
    {
        croak("Unknown target: $targetName");
    }
    my ($includes, $excludes, $platforms) = TargetAttributesResolver::resolveDependencies($target);
    $target->setResolvedValues(\@$includes, \@$excludes, \@$platforms);
    $this->{target} = $target;
    return $this->{target};
}

sub getTestPlan
{
    my ($this) = @_;

    if (defined($this->{testPlan}))
    {
        return $this->{testPlan};
    }
    my $testList = $this->getOptionValue('testlist');
    if ($testList)
    {
        $this->{testPlan} = TestPlan::loadFromPlainTestListFile($testList);
        return $this->{testPlan};
    }

    my $partitionSource = $this->getOptionValue('partition-source');
    if ($partitionSource)
    {
        my $partitionIndex = $this->getOptionValue('partition-index');
        Logger::minimal('partition-index: $partitionIndex');
        $this->{testPlan} = TestPlan::loadFromPartitionsJsonFile($partitionSource, $partitionIndex, $this->getOptionValue('testfilter'));

        Logger::minimal("Total cases: " . $this->getTestPlan()->getTotalTestCases());
        return $this->{testPlan};
    }

    my $partionsId = $this->getOptionValue('partitions-id');
    if ($partionsId)
    {
        my $partitionIndex = $this->getOptionValue('partition-index');
        my $partionsId = $this->getOptionValue('partitions-id');
        my $jsonReponse = ParallelTestsSelector::queryPartitionsById(
            id => $this->getOptionValue('partitions-id'),
            uri => $this->getOptionValue('parallel-tests-selector-uri'),
            timeout => $this->getOptionValue('query-partitions-timeout'),
            retries => $this->getOptionValue('query-partitions-retries')
        );
        $this->{testPlan} = TestPlan::loadFromPartitionsJson($jsonReponse, $partitionIndex, $this->getOptionValue('testfilter'));
        Logger::minimal("Total cases: " . $this->getTestPlan()->getTotalTestCases());
        return $this->{testPlan};
    }

    my @cmdArgs = ();

    push(@cmdArgs, '--suite=integration');
    push(@cmdArgs, '--list');
    push(@cmdArgs, "--artifacts_path=" . $this->getArtifactsPath());
    push(@cmdArgs, '--no-stdout');
    $this->_addRunnerOptions(\@cmdArgs);
    my $target = $this->resolveTarget();
    $this->pushIncludesExcludes(\@cmdArgs, $target);

    my $filter = $this->getOptionValue('testfilter');
    if (defined($filter))
    {
        push(@cmdArgs, "--testfilter=$filter");
    }

    my @dlls = $this->getAssemblyWithTests();
    push(@cmdArgs, @dlls);

    my $testRunner = $this->_getAssemblyFullPath($unifiedTestRunnerCSharpProj);
    my $exitCode = $this->runDotNetProgram(
        program => $testRunner,
        programArgs => \@cmdArgs
    );

    if ($exitCode != 0)
    {
        croak("Failed to build a list of tests to run");
    }

    my $testRunnerLog = catfile($this->getArtifactsPath(), 'TestRunnerLog.json');
    my $testPlanMsg = UnityTestProtocol::Utils::getFirst(
        fileName => $testRunnerLog,
        predicate => sub
        {
            my ($msg) = @_;
            return $msg->{type} eq 'TestPlan';
        }
    );

    $this->{testPlan} = TestPlan::getTestPlanFromTestProtocolMessage($testPlanMsg);
    if (not $this->getOptionValue('query-partitions'))
    {
        $this->{testPlan} = $this->applySmartSelect($this->{testPlan});
    }

    unlink($testRunnerLog);
    my @testNames = $this->{testPlan}->getTestNames();
    return $this->{testPlan};
}

sub readTestPlanFromList
{
    my ($this, $testListFileName) = @_;

    my @tests = FileUtils::readAllLines($testListFileName);
    my $testPlan = new TestPlan();
    foreach my $t (@tests)
    {
        my $testPlanItem = TestPlanItem->new(name => $t, testCaseCount => 1);
        $testPlan->registerTest($testPlanItem);
    }
    return $testPlan;
}

sub pushIncludesExcludes
{
    my ($this, $args, $target) = @_;

    my @includes = $target->getIncludes();
    foreach my $inc (@includes)
    {
        push(@{$args}, "--include=$inc");
    }

    my @excludes = $target->getExcludes();
    foreach my $exc (@excludes)
    {
        push(@{$args}, "--exclude=$exc");
    }
}

sub getAssemblyWithTests
{
    my ($this) = @_;
    my @testProjectPaths = $this->resolveTestProjects();
    return $this->getDllNames(@testProjectPaths);
}

sub getDllNames
{
    my ($this, @projects) = @_;
    my @expected_dlls = ();
    foreach my $project (@projects)
    {
        push(@expected_dlls, $this->_getAssemblyFullPath($project));
    }
    return @expected_dlls;
}

sub resolveTestProjects
{
    my ($this) = @_;
    my @projectNames = @{ $this->getOptionValue('testprojects') };

    # temporary until we split all into smaller test suites
    # will be splittet later
    if (scalar(@projectNames) == 1 and lc($projectNames[0]) eq 'all')
    {
        @projectNames = $this->_getAllCSProjects();
    }

    my $root = Dirs::getRoot();
    my $slnFile = $this->_getSlnFileName();
    my @allProjects = getAllProjects($slnFile);

    my @projects;
    foreach my $project (@projectNames)
    {
        my @projectPaths = resolveProjectPath($project, @allProjects);
        if (scalar(@projectPaths) == 0)
        {
            croak("Can not resolve project path for $project");
        }
        push(@projects, @projectPaths);
    }

    return uniq(@projects);
}

sub _getSlnFileName
{
    my ($this) = @_;
    return canonpath("$root/Projects/CSharp/Unity.CSharpProjects.gen.sln");
}

sub getAllProjects
{
    my ($slnFile) = @_;
    my $f = openForReadOrCroak($slnFile);
    my @projects = _getProjects($f, $slnFile);
    closeOrCroak($f, $slnFile);
    return @projects;
}

sub _getProjects
{
    my ($fh, $slnFile) = @_;
    my $slnPath = dirname($slnFile);
    my @projects;
    while (<$fh>)
    {
        my ($projectPath) = ($_ =~ m /^Project.*"(.*csproj)"/is);
        if ($projectPath)
        {
            chomp($projectPath);
            $projectPath =~ s/\\/\//gms;
            my $isAbsolute = File::Spec->file_name_is_absolute($projectPath);
            if (!$isAbsolute)
            {
                $projectPath = realpath(File::Spec->catfile($slnPath, $projectPath));
            }

            push(@projects, canonpath($projectPath));
        }
    }
    return @projects;
}

sub resolveProjectPath
{
    my ($project, @projects) = @_;
    my @result;
    Readonly my $NOT_FOUND_IDX => -1;
    foreach (@projects)
    {
        if (index(uc($_), uc($project)) != $NOT_FOUND_IDX)
        {
            push(@result, $_);
        }
    }
    return @result;
}

sub getUnityArtifactsFolder
{
    my ($this) = @_;
    my $artifactsFolder = $this->getOptionValue('unity-artifacts-folder');
    if (defined($artifactsFolder))
    {
        return canonpath($artifactsFolder);
    }

    return $this->{artifactsNameBuilder}->getFullFolderPathForArtifactType(type => 'Artifacts');
}

sub projectPath
{
    my ($this) = @_;
    my $projectPath = $this->getOptionValue('projectpath');
    if (defined($projectPath))
    {
        return canonpath($projectPath);
    }
    return $this->{artifactsNameBuilder}->getFullFolderPathForArtifactType(type => 'Project');
}

sub startNewIteration
{
    my ($this) = @_;
    $this->{artifact_folders_prepared} = 0;
}

sub getArtifactsMap
{
    my ($this) = @_;
    return ArtifactsMapBuilder::fromGraphicsTestRunnerStdout($this->getMachineLogPath(), $this->getArtifactsPath());
}

sub prepareBuildTargets
{
    my ($this) = @_;
    my @platforms = $this->getRuntimePlatforms();
    foreach my $platform (@platforms)
    {
        $this->prepareBuildTarget($platform);
    }
}

sub buildAutomationDependencies
{
    my ($this) = @_;
    my @platforms = $this->getRuntimePlatforms();
    foreach my $platform (@platforms)
    {
        my @projects = AutomationPlayer::resolve($platform);
        foreach my $p (@projects)
        {
            $this->buildCSProject($p);
        }
    }
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

sub _collectTargets
{
    my $teamcityProjectPath = canonpath("$root/Projects/CSharp/TeamCityTests.proj");
    my $resolver = new TargetAttributesResolver($teamcityProjectPath);

    my @allTargets = $resolver->getAllTargetNames();

    @allTargets = grep { not _shouldHideTarget($_) } @allTargets;

    return sort (@allTargets);
}

sub stdOutFileName
{
    return undef;
}

sub stdErrFileName
{
    return undef;
}

sub testResultXmlFileName
{
    return undef;
}

sub processResults
{
    my ($this) = @_;
    my $jsonReportPath = catfile($this->getArtifactsPath(), 'TestReportData.json');
    my $testReport = FileUtils::readJsonObj($jsonReportPath);
    foreach my $suite (@{ $testReport->{suites} })
    {
        my $suiteResult = SuiteResult::fromJson($suite);
        $this->writeSuiteData($suiteResult);
        $this->sendResultToConsumer($suiteResult);
    }
}

sub _shouldHideTarget
{
    my ($targetName) = @_;
    my @targetsToHide = (
        'Build', 'Prepare', 'RunTest', 'BuildForIMGUITests', 'RunIMGUIControlTest', 'CleanupTestsBin', 'InstallEditor',
        'TryToTestIMGUIControlTestsWithNUnitConsole',
        'TryToTestIMGUIControlTestsWithNUnitTask',
        'TryToTestWithNUnitConsole', 'TryToTestWithNUnitTask', 'TestALL'
    );

    if (grep(/^$targetName$/, @targetsToHide))
    {
        return 1;
    }

    return 0;
}

sub getPathToReporter
{
    my ($this) = @_;
    return $this->_getAssemblyFullPath($reporterProj);
}

sub reporterStdOutFileName
{
    return performanceReporterStdOutFileName(@_);
}

sub reporterStdErrFileName
{
    return performanceReporterStdErrFileName(@_);
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
    return $this->{artifactsNameBuilder}->getFullAtrifactFileName(
        'parent-type' => 'Unity.PerformanceTests.Reporter',
        type => 'perfdata',
        extension => 'json'
    );
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

sub testPlanFileName
{
    my ($this) = @_;
    return $this->{artifactsNameBuilder}->getFullAtrifactFileName(
        'parent-type' => 'UnifiedTestRunner',
        type => 'TestPlan',
        extension => 'txt'
    );
}

1;
