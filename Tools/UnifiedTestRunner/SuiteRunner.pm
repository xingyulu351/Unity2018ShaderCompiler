package SuiteRunner;

use parent Plugin;

use warnings;
use strict;

use Carp qw (croak);
use File::Spec::Functions qw (canonpath catfile catdir);
use File::Basename qw (fileparse);
use File::Path qw (mkpath);

use Environments::Mono;
use Artifacts;
use SuiteTools;
use FolderDeleter;
use ArtifactsFolderDeleterGuard;
use SuiteResult;
use JamWrapper;
use FileUtils;
use FileWatcher;
use Logger;
use NUnitXmlParser;
use FileUtils qw (openForReadOrCroak openForWriteOrCroak closeOrCroak readAllLines);
use BuildTargetResolver;
use PlatformTarget;
use AutomationPlayer;
use File::Basename qw (basename dirname);
use lib Dirs::external_root();
use Try::Tiny;
use CSProj;
use VSSolution;
use FolderDeleter;
use Logger;
use RepositoryInfo;
use SmartTestsSelector;
use RemoteEventLogger;
use UnityTestProtocol::Utils;
use UnityTestProtocol::Messages;
use Term::ANSIColor qw( colored );
use Term::ANSIColor qw(:constants);
use PerformanceReporter;
use Capabilities;

sub run
{
    my ($this) = @_;
    $this->_prepareOptions();
    $this->registerStaticArtifacts();
    $this->cleanPreviousLogsIfAny();
    $this->prepareArtifactsFolders();

    $this->prepareEnv();
    my $skipBuildDependencies = $this->getOptionValue('skip-build-deps');
    if (not $skipBuildDependencies)
    {
        my $startTime = time();
        $this->buildDependencies();
        $this->buildAutomationDependencies();
        my $duration = time() - $startTime;
        Logger::minimal("Done building dependencies. $duration seconds.");
    }
    $this->beforeRun();
    $this->doRun();
    $this->afterRun();
    $this->processResults();
}

sub updateRunnerOptions
{
    my ($this) = @_;
    my $updateOptions = $this->getOptionValue('update-runner-options');
    if ($updateOptions)
    {
        $this->updateRunnerHelp();
    }
}

sub list
{
    my ($this) = @_;
    $this->prepareArtifactsFolders();
    my $skipBuildDependencies = $this->getOptionValue('skip-build-deps');
    if (not $skipBuildDependencies)
    {
        my $startTime = time();
        $this->buildDependencies();
        my $duration = time() - $startTime;
        Logger::minimal("Done building dependencies. $duration seconds.");
    }
    my $plan = $this->getTestPlan();
    return $plan->getTestNames();
}

sub buildPlayer
{
    my ($this) = @_;
    $this->registerStaticArtifacts();
    $this->cleanPreviousLogsIfAny();
    $this->prepareArtifactsFolders();
    $this->buildDependencies();
    $this->buildAutomationDependencies();
    $this->doBuildPlayer();
}

sub doBuildPlayer
{

}

sub getTestPlan
{
    my ($this) = @_;
    croak("Test plan is not supported for " . $this->getName() . " suite");
}

sub registerStaticArtifacts
{
    croak("Method 'registerStaticArtifacts' must be implemented in subclasses");
}

sub cleanPreviousLogsIfAny
{
    my ($this) = @_;
    ArtifactsFolderDeleterGuard::safeFolderDelete(
        folder => $this->getSuiteRunArtifactsRoot(),
        suite => $this->getName(),
        platform_or_target => $this->getPlatform(),
        deleter => new FolderDeleter()
    );
}

sub prepareArtifactsFolders
{
    croak("Method 'prepareArtifactsFolders' must be implemented in subclasses");
}

sub buildDependencies
{

    #can be redefined / implemented in subclasses
}

sub getArtifactsMap
{
    return new ArtifactsMap();
}

sub processResults
{
    my ($this) = @_;
    my $buildOnly = $this->getOptionValue('--build-only');
    if ($buildOnly)
    {
        return;    # no results expected
    }
    my $testresultsxml = $this->testResultXmlFileName();

    my $suiteResult = $this->createSuiteResult($testresultsxml);
    $this->writeSuiteData($suiteResult);
    $this->sendResultToConsumer($suiteResult);
}

sub sendResultToConsumer
{
    my ($this, $suiteResult) = @_;
    my $resultConsumer = $this->getResultConsumer();
    if ($resultConsumer)
    {
        $resultConsumer->addSuiteResult($suiteResult);
    }
}

sub createSuiteResult
{
    my ($this, $testresultsxml) = @_;
    my @results;
    my $error = undef;
    try
    {
        my %parseRes = $this->parseTestResultsXml($testresultsxml);
        @results = @{ $parseRes{testResults} };
    }
    catch
    {
        my $testresultsxml = FileUtils::removeSubPath($testresultsxml, $this->getArtifactsPath());
        $error = "'$testresultsxml'. Does not exist. ";
        $error .= "This is most likely due to the previous failures. ";
        $error .= "Check the available artifacts";
    };
    my @artifacts = $this->getArtifacts()->get($this->getName());

    my $suiteResult;
    if (defined($error))
    {
        $suiteResult = new SuiteResult(
            name => $this->getName(),
            platform => $this->getPlatform(),
            testresultsxml => undef,
            partitionIndex => $this->getOptionValue('partition-index'),
            artifactsmap => undef,
            artifacts => \@artifacts
        );
    }
    else
    {
        $suiteResult = new SuiteResult(
            name => $this->getName(),
            platform => $this->getPlatform(),
            testresultsxml => $testresultsxml,
            artifactsmap => $this->getArtifactsMap(),
            partitionIndex => $this->getOptionValue('partition-index'),
            artifacts => \@artifacts
        );
    }

    if (defined($error))
    {
        $suiteResult->forceFailed($error);
    }

    if ($this->{runnerExitCode} != 0)
    {
        my $runnerExitCodeHex = sprintf("0x%X", $this->{runnerExitCode});
        $suiteResult->forceFailed("Runner exited with non zero exit code $this->{runnerExitCode} ($runnerExitCodeHex)");
    }

    $suiteResult->addTestResults(@results);
    $suiteResult->setZeroTestsAreOk($this->getOptionValue('zero-tests-are-ok'));

    return $suiteResult;
}

sub applyArtifactsMapToTestResults
{
    my ($this, @results) = @_;
    my $artifactsMap = $this->getArtifactsMap();
    foreach my $r (@results)
    {
        my $testName = $r->getTest();
        $r->setArtifacts($artifactsMap->getArtifactsForTest($testName));
    }
    return @results;
}

sub writeSuiteData
{

    #my ($this, $suiteResult) = @_;
    #do nothing
}

sub parseTestResultsXml
{
    my ($this, $testresultsxml) = @_;
    return NUnitXmlParser::parse($testresultsxml);
}

sub report
{
    my ($this, $suiteResult) = @_;
    my $resultConsumer = $this->getResultConsumer();
    if (not defined($resultConsumer))
    {
        return;
    }
    $resultConsumer->addSuiteResult($_);
}

sub setResultConsumer
{
    my ($this, $consumer) = @_;
    $this->{consumer} = $consumer;
}

sub getResultConsumer
{
    my ($this) = @_;
    return $this->{consumer};
}

sub setProgressPresenter
{
    my ($this, $value) = @_;
    $this->{progress} = $value;
}

sub getProgressPresenter
{
    my ($this) = @_;
    return $this->{progress};
}

sub getSuiteRunArtifactsRoot
{
    my ($this) = @_;
    return $this->{artifactsNameBuilder}->getSuiteRunRoot();
}

sub getSuiteArtifactsFolderName
{
    my ($this, $artifactsType) = @_;
    return $this->{artifactsNameBuilder}->getFullFolderPathForArtifactType(type => $artifactsType);
}

sub getFullArtifactFileName
{
    my $this = shift;
    my %params = @_;

    my $baseFolder = $this->getSuiteArtifactsFolderName($params{'parent-type'});
    my $filename = ArtifactsNameBuilder::getName(
        type => $params{'type'},
        extension => $params{'extension'}
    );

    return canonpath("$baseFolder/$filename");
}

sub stdOutFileName
{
    croak("Method 'stdOutFileName' must be implemented in subclasses");
}

sub stdErrFileName
{
    croak("Method 'stdErrFileName' must be implemented in subclasses");
}

sub testPlanFileName
{
    croak("Method 'testPlanFileName' must be implemented in subclasses");

}

sub testResultXmlFileName
{
}

sub startNewIteration
{
    croak("Method 'startNewIteration' must be implemented in subclasses");
}

sub writeTestPlan
{
    my ($this, $filename, $testPlan) = @_;
    my $fh = $this->_openFileForWrite($filename);
    foreach ($testPlan->getAllTestItems())
    {
        my $testName = $_->getName();
        print($fh "$testName\n");
    }
    close($fh);
}

sub getTool
{
    my ($this) = @_;
    my $toolName = $this->getOptionValue('tool');
    if (not defined($toolName))
    {
        return;
    }
    my $tool = SuiteTools::getTool($toolName);
    $tool->setRunContext($this->{runContext});
    $tool->setArtifactsNameBuilder($this->getArtifactsNameBuilder());
    $tool->setArtifacts($this->{artifacts});
    return $tool;
}

sub registerArtifacts
{
    my ($this, @artifacts) = @_;
    foreach my $a (@artifacts)
    {
        $this->registerArtifact($a);
    }
}

sub buildCSProject
{
    my ($this, $project, $framework) = @_;

    my $baseFolder = $this->getSuiteArtifactsFolderName('UnifiedTestRunner');
    my $isolated = $this->getOptionValue('build-isolation') eq 'enabled';
    my $projectName = fileparse($project);
    my $logName = $this->getFullArtifactFileName(
        'parent-type' => 'UnifiedTestRunner',
        type => "$projectName-BuildLog",
        extension => 'txt'
    );

    my $buildMode = $this->getOptionValue('build-mode');
    my $rebuild = 0;
    if ($buildMode eq 'full')
    {
        $rebuild = 1;
    }

    my $outputPath = undef;
    if ($isolated)
    {
        $outputPath = $this->_getOutputBuildPath($project);
    }

    my $environment = $this->getEnvironment();
    my $buildEngine = $environment->getBuildEngine();
    $buildEngine->build(
        config => "Debug",
        project => $project,
        rebuild => $rebuild,
        logfile => $logName,
        framework => $framework,
        outputPath => $outputPath
    );
}

sub buildCSProject2
{
    my ($this, $project, $framework) = @_;
    my $baseFolder = $this->getSuiteArtifactsFolderName('UnifiedTestRunner');
    my $isolated = $this->getOptionValue('build-isolation') eq 'enabled';
    my $projectName = fileparse($project);
    my $logName = $this->getFullArtifactFileName(
        'parent-type' => 'UnifiedTestRunner',
        type => "$projectName-BuildLog",
        extension => 'txt'
    );

    my $buildMode = $this->getOptionValue('build-mode');
    my $environment = $this->getEnvironment();
    my $buildEngine = $environment->getBuildEngine();
    $buildEngine->build(
        config => "Debug",
        project => $project,
        rebuild => 0,
        logfile => $logName,
        framework => $framework,
    );
}

sub cleanCSharpProjectsSln
{
    my ($this) = @_;

    my $buildMode = $this->getOptionValue('build-mode');
    if (not $buildMode eq 'full')
    {
        return;
    }
    my $root = Dirs::getRoot();
    my $slnFile = canonpath("$root/Projects/CSharp/Unity.CSharpProjects.gen.sln");
    my $isolated = $this->getOptionValue('build-isolation') eq 'enabled';
    if ($isolated)
    {
        my $vsSolution = new VSSolution($slnFile);
        my @projects = $vsSolution->getCSProjects();
        foreach my $prj (@projects)
        {
            my $outputPath = $this->_getOutputBuildPath($prj->getProjectPath());
            if (-d $outputPath)
            {
                Logger::minimal("Deleting '$outputPath'");
                my $deleter = new FolderDeleter();
                $deleter->delete($outputPath);
            }
        }
        return;
    }

    $this->cleanCSProject($slnFile);
}

sub cleanCSProject
{
    my ($this, $project) = @_;
    my $buildMode = $this->getOptionValue('build-mode');
    my $environment = $this->getEnvironment();
    my $buildEngine = $environment->getBuildEngine();
    $buildEngine->clean($project);
}

sub applySmartSelect
{
    my ($this, $testPlan) = @_;

    my $smartSelect = $this->getOptionValue('smart-select');
    if ($smartSelect)
    {
        my $configId = $this->getOptionValue('config-id');
        my $submitter = $this->getOptionValue('submitter');
        my $uri = $this->getOptionValue('smart-tests-selector-uri');
        my $build = $this->getOptionValue('build-number');
        my $capabilities = Capabilities::getCapabilities();

        my ($filteredTestRun, $skippedTests, $id) = SmartTestsSelector::queryTests(
            RepositoryInfo::get(),
            uri => $uri,
            configId => $configId,
            submitter => $submitter,
            testPlan => $testPlan,
            build => $build,
            capabilities => $capabilities
        );

        $this->{runContext}->setSmartSelectSessionId($id);

        if ($filteredTestRun)
        {
            my $totalTestsCount = $testPlan->getTotalTests();

            $testPlan = $filteredTestRun;
            my $skippedTestsFileName = $this->skippedTestsFileName();
            $this->_writeSkippedTests($skippedTests, $skippedTestsFileName);
            $this->registerArtifacts($skippedTestsFileName);

            my $skippedTestsCount = scalar(@{$skippedTests});
            if ($skippedTestsCount > 0)
            {
                my $smartSelectData = {
                    total => $totalTestsCount,
                    skipped => $skippedTestsCount,
                };
                my $msg = UnityTestProtocol::Messages::makeSmartSelectMessage($smartSelectData);
                RemoteEventLogger::log($msg);
            }
        }
        else
        {
            Logger::error("Running the full test plan due to inability to retrieve statistical information.");
        }
    }

    return $testPlan;
}

sub skippedTestsFileName
{
    my ($this) = @_;
    return $this->{artifactsNameBuilder}->getFullAtrifactFileName(
        'parent-type' => 'UnifiedTestRunner',
        'type' => 'TestsExcludedBySmartSelect',
        'extension' => 'txt'
    );
}

sub _writeSkippedTests
{
    my ($this, $skippedTests, $fileName) = @_;
    FileUtils::writeAllLines($fileName, @{$skippedTests});
}

sub _getAssemblyFullPath
{
    my ($this, $project) = @_;
    my $isolated = $this->getOptionValue('build-isolation') eq 'enabled';
    my $csProj = new CSProj($project);
    if (not $isolated)
    {
        return $csProj->getAssemblyFullPath();
    }
    my $assemblyDir = FileUtils::getDirName($project);
    my $outputPath = $this->_getOutputBuildPath($project);
    return catfile($outputPath, $csProj->getAssemblyNameWithExtenstion());
}

sub _openFileForRead
{
    my ($this, $filename) = @_;
    return openForReadOrCroak($filename);
}

sub _openFileForWrite
{
    my ($this, $filename) = @_;
    return openForWriteOrCroak($filename);
}

sub _getOutputBuildPath
{
    my ($this, $project) = @_;
    my $environment = $this->getEnvironment();
    my $buildEngine = $environment->getBuildEngine();
    my $root = Dirs::getRoot();
    my $csProj = new CSProj($project);
    my $outputPath = $csProj->getFullOutputPath("Debug");
    my $env = $this->getEnvironment();
    my $debugFolder = "UTRDebug_" . $env->getName() . '_' . $buildEngine->getName() . "/";
    $outputPath =~ s/([\/\\])debug/$1$debugFolder/i;
    return canonpath($outputPath);
}

sub createArtifactsDirIfNotExistsOrCroak
{
    my ($this, $type) = @_;
    my $dir = $this->getSuiteArtifactsFolderName($type);
    if (-d $dir)
    {
        return;
    }

    if (not mkpath($dir))
    {
        croak("Can not create dir: $dir");
    }
}

sub jam
{
    my ($this, @targets) = @_;
    $this->createArtifactsDirIfNotExistsOrCroak('Jam');
    for my $target (@targets)
    {
        my $jamStdOutFileName = $this->getFullArtifactFileName(
            'parent-type' => 'Jam',
            type => "$target",
            extension => 'txt'
        );

        $this->registerArtifact($jamStdOutFileName);

        my $fw = new FileWatcher();
        $fw->watch($jamStdOutFileName, \&Plugin::printLogMessage, $this);
        $fw->start();
        JamWrapper::Jam($target, $jamStdOutFileName);
        $fw->stop();
    }
}

sub prepareEnv
{

    # do nothing
}

sub beforeRun
{

    #do nothing
}

sub afterRun
{

    #do nothing
}

sub prepareBuildTarget
{
    my ($this, $platform) = @_;
    $this->_executeBuildTargetAction($platform, 'Prepare');
}

sub disposeBuildTarget
{
    my ($this, $platform) = @_;
    $this->_executeBuildTargetAction($platform, 'Dispose');
}

sub _executeBuildTargetAction
{
    my ($this, $platform, $action) = @_;
    my $target = BuildTargetResolver::resolveTarget($platform);
    if (not defined($target))
    {
        return;
    }
    my $module = $target->getModule();
    my $stdOut = $this->buildTargetActionStdOutName($module, $action);
    my $stdErr = $this->buildTargetActionStdErrName($module, $action);
    my $shortTargetName = basename($module);
    Logger::minimal("$action build target $shortTargetName :\n\t$stdOut\n\t$stdErr");
    local *STDOUT;
    local *STDERR;
    open(STDOUT, '>', $stdOut);
    open(STDERR, '>', $stdErr);
    my $options = {
        scriptingBackend => $this->getOptionValue('scriptingbackend', 'default'),
        test => 1,
        suite => $this->getName()
    };

    if (lc($action) eq 'prepare')
    {
        $target->prepare($options);
    }
    elsif (lc($action) eq 'dispose')
    {
        $target->dispose($options);
    }
}

sub buildAutomationDependencies
{
    my ($this) = @_;
    my $platform = $this->getPlatform();
    my @projects = AutomationPlayer::resolve($platform);
    foreach my $p (@projects)
    {
        $this->buildCSProject($p);
    }
}

sub buildTargetActionStdOutName
{
    my ($this, $module, $operation) = @_;
    return $this->{artifactsNameBuilder}->getFullAtrifactFileName(
        'parent-type' => 'UnifiedTestRunner',
        type => basename($module) . ".$operation.stdout",
        extension => 'txt'
    );
}

sub buildTargetActionStdErrName
{
    my ($this, $module, $operation) = @_;
    return $this->{artifactsNameBuilder}->getFullAtrifactFileName(
        'parent-type' => 'UnifiedTestRunner',
        type => basename($module) . ".$operation.stderr",
        extension => 'txt'
    );
}

sub isRunningMultipleSuites
{
    my ($this) = @_;
    my $runContext = $this->{runContext};
    my $suites = $this->getOptionValue('suite');
    if ($suites and scalar(@{$suites}) > 1)
    {
        return 1;
    }
    return 0;
}

sub testResultXmlFileNameImpl
{
    my ($this, $parentType) = @_;

    my $testresultsxml = $this->getOptionValue('testresultsxml');

    my $multipleSuites = $this->isRunningMultipleSuites();
    if ($testresultsxml and $multipleSuites)
    {
        Logger::warning("'testresultsxml' parameter is ambiguous when running multiple suites. Ignoring.");
    }

    if ($testresultsxml and not $multipleSuites)
    {
        return canonpath($testresultsxml);
    }

    return $this->{artifactsNameBuilder}->getFullAtrifactFileName(
        'parent-type' => $parentType,
        type => 'TestResults',
        extension => 'xml'
    );
}

sub testResultXmlFormatImpl
{
    my ($this, $parentType) = @_;

    my $testresultsformat = $this->getOptionValue('testresultsformat');

    return $testresultsformat;
}

sub watchTestProtocolMessages
{
    my ($line, $this, $filename) = @_;
    my $msg = UnityTestProtocol::Utils::getMessage($line);
    if ($msg)
    {
        $this->onMessage($msg);
        RemoteEventLogger::log($msg);
    }
}

sub processTestFinishMessage
{
    my ($this, $msg) = @_;
    if (   $msg->{state} eq TestResult::FAILURE
        or $msg->{state} eq TestResult::ERROR
        or $msg->{state} eq TestResult::INCONCLUSIVE)
    {
        my $message = $msg->{message} || '';
        my $testName = $msg->{name};
        if ($msg->{classname} and index($msg->{classname}, $testName) != -1)
        {
            $testName = "$msg->{classname}.$testName";
        }
        my $suite = $this->getName();
        my $state = TestResult::stateToString($msg->{state});
        my $testFailMessage = "$testName $suite test has failed.\nState: $state\nMessage: $message";
        chomp($testFailMessage);
        if ($msg->{namespace})
        {
            $testFailMessage .= "\nClass name: $msg->{source}\n";
        }
        Logger::minimal(colored($testFailMessage, 'bright_red'));
    }
}

sub processTestStatusMessage
{
    my ($this, $msg) = @_;
    if ($msg->{phase} eq 'Begin')
    {
        $this->processTestStartMessage($msg);
    }

    if ($msg->{phase} eq 'End')
    {
        $this->processTestFinishMessage($msg);
    }
}

sub onMessage
{
    #do nothing
}

sub buildCSDependencies
{
    my ($this, @projects) = @_;
    if ($this->{dependencies_built})
    {
        return;
    }
    my $msbuildProjFileName = $this->_generateMSBuildScript(@projects);
    $this->buildCSProject2($msbuildProjFileName);
    $this->{dependencies_built} = 1;
}

sub isEditor
{
    my ($this) = @_;
    return $this->getPlatform() =~ m/editor/i;
}

sub _generateMSBuildScript
{
    my ($this, @projects) = @_;
    my $generator = new MSBuildProjectGenerator();
    my $isolated = $this->getOptionValue('build-isolation') eq 'enabled';

    foreach my $tp (@projects)
    {
        my $csProj = new CSProj($tp);
        my $outputPath = $csProj->getFullOutputPath('Debug');
        if ($isolated)
        {
            $outputPath = $this->_adjustOutputPath($csProj);
        }
        $generator->addProject($csProj, $outputPath);
    }

    my $env = $this->getEnvironment();
    my $buildEngine = $env->getBuildEngine();
    my $msbuildProjFileName = $this->{artifactsNameBuilder}->getFullAtrifactFileName(
        'parent-type' => 'UnifiedTestRunner',
        type => 'msbuild',
        extension => 'proj'
    );

    Logger::minimal("Generating $msbuildProjFileName for projects:\n\t" . join("\n\t", map { _removeRoot($_) } @projects));
    my $intermidiateBuildFolder = "UTRDebug_Intermediate_" . $env->getName() . '_' . $buildEngine->getName() . "/";
    my $root = Dirs::getRoot();
    if ($isolated)
    {
        $generator->setBaseIntermediateOutputPath("$root/Tests/bin/$intermidiateBuildFolder");
    }
    $generator->generate($msbuildProjFileName);
    return $msbuildProjFileName;
}

sub _reportPerformanceData
{
    my ($this) = @_;
    PerformanceReporter::report(
        $this->getEnvironment(),
        configName => $this->getOptionValue('config'),
        suiteName => $this->getReporterSuiteName(),
        reporterPath => $this->getPathToReporter(),
        platform => $this->getPlatform(),
        testResultsXml => $this->testResultXmlFileName(),
        outputJson => $this->performanceDataFileName(),
        csvResultsFile => $this->getOptionValue('testresultscsv'),
        forceBaselineUpdate => $this->getOptionValue('forcebaselineupdate'),
        stdOut => $this->reporterStdOutFileName(),
        stdErr => $this->reporterStdErrFileName(),
    );
}

sub _removeRoot
{
    my ($file) = @_;
    my $root = Dirs::getRoot();
    return FileUtils::removeSubPath($file, $root);
}

sub _adjustOutputPath
{
    my ($this, $csProj) = @_;
    my $projectPath = $csProj->getProjectPath();
    my $outputPath = $csProj->getFullOutputPath("Debug");
    my $env = $this->getEnvironment();
    my $buildEngine = $env->getBuildEngine();
    my $debugFolder = "UTRDebug_" . $env->getName() . '_' . $buildEngine->getName() . "/";
    $outputPath =~ s/([\/\\])debug/$1$debugFolder/i;
    return $outputPath;
}

1;
