package UnityTestFrameworkSuiteRunnerBase;

use parent SuiteRunner;

use warnings;
use strict;

use File::Find;
use File::Spec::Functions qw (catfile canonpath catdir splitdir rel2abs);
use File::Path qw (mkpath remove_tree);
use File::Copy::Recursive qw (dircopy);
use List::MoreUtils qw (uniq);
use NUnitXmlParser;

use Logger;
use Dirs;
use lib Dirs::external_root();
use JSON;
use FileUtils;
use File::Basename;
use UnityTestProtocol::Utils qw(:all);

use Switch;
my $root = Dirs::getRoot();

use constant {
    RETURN_CODE_TESTS_OK => 0,
    RETURN_CODE_TESTS_FAILED => 2,
    RETURN_CODE_NO_TESTS_FOUND => 3,
    RETURN_CODE_FAILED_TO_RESOLVE_PROJECT_PATH => 4
};

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
        my $projectName = getLastFolder($projectPath);
        switch ($projectExitCode)
        {
            case RETURN_CODE_TESTS_OK { Logger::minimal("Completed tests for project $projectName ExitCode:$projectExitCode"); }
            case RETURN_CODE_TESTS_FAILED { Logger::minimal("Failed tests for project $projectName ExitCode:$projectExitCode"); }
            case RETURN_CODE_NO_TESTS_FOUND { Logger::warning("No tests for project $projectName"); }
            else
            {
                Logger::error("Unknown exitcode $projectExitCode while running $projectName");
            }
        }
        $this->afterProjectRun($this->getOptionValue('platform'));
    }
}

sub getPlatform
{
    my ($this) = @_;
    return $this->getName();
}

sub doRunUnityProject
{
    my ($this, $projectPath) = @_;
    my $options = getProjectOptions($projectPath);
    if ($options->{disabled})
    {
        Logger::minimal("Skipping '$projectPath' as it is marked as 'disabled' in 'TestRunnerOptions.json'");
        return 0;
    }

    my $platform = $this->getOptionValue('platform');
    if ($options->{runOnlyInEditor} and not($platform eq 'editmode' or $platform eq 'playmode'))
    {
        Logger::minimal("Skipping '$projectPath' as it is marked as 'runOnlyInEditor' in 'TestRunnerOptions.json' and the platform is $platform.");
        return 0;
    }

    if (exists($options->{allowedPlatforms}) and !(grep { /$platform$/i } @{ $options->{allowedPlatforms} }))
    {
        Logger::minimal("Skipping '$projectPath' as it is not marked as 'allowedPlatforms' in 'TestRunnerOptions.json' and the platform is $platform.");
        return 0;
    }

    my $scriptingRuntimeVersion = $this->getOptionValue('use-legacy-scripting-runtime-version');
    if ($options->{disabledOnLegacyScriptingRuntime} and defined($scriptingRuntimeVersion))
    {
        Logger::minimal("Skipping '$projectPath' as it is marked as 'disabledOnLegacyScriptingRuntime' in 'TestRunnerOptions.json'");
        return 0;
    }

    if ($options->{disabledOnLatestScriptingRuntime} and not defined($scriptingRuntimeVersion))
    {
        Logger::minimal("Skipping '$projectPath' as it is marked as 'disabledOnLatestScriptingRuntime' in 'TestRunnerOptions.json'");
        return 0;
    }

    if ($options->{disabledOnLegacyScriptingRuntime} and $this->getOptionValue('use-legacy-scripting-runtime-version'))
    {
        Logger::minimal("Skipping '$projectPath' as it is marked as 'disabledOnLegacyScriptingRuntime' in 'TestRunnerOptions.json'");
        return 0;
    }

    my $scriptingBackend = $this->getOptionValue('scripting-backend');
    if ($options->{disabledOnIL2CPPScriptingBackend} and ($scriptingBackend eq 'IL2CPP'))
    {
        Logger::minimal("Skipping '$projectPath' as it is marked as 'disabledOnIL2CPPScriptingBackend' in 'TestRunnerOptions.json'");
        return 0;
    }

    if (projectHasTestsForPlatform($projectPath, $platform) eq 0)
    {
        Logger::minimal("Skipping '$projectPath' as the content of the project is determined to not contain tests for '$platform'.");
        return 0;
    }

    my @cmdArgs;
    push(@cmdArgs, TargetResolver::resolve('editor'));

    my $projectName = getLastFolder($projectPath);
    my $testresultsxml = $this->testResultXmlFileName($projectName);
    if (defined($this->getOptionValue('assetpipelinev2')))
    {
        if (not $options->{allowAssetPipelineV2})
        {
            Logger::minimal("Skipping '$projectPath' as it is not marked as 'allowAssetPipelineV2' in 'TestRunnerOptions.json' and the test run requires it");
            return 0;
        }
        else
        {
            push(@cmdArgs, '-assetpipelinev2');
            push(@cmdArgs, '-ImportModeForced');
        }
    }

    push(@cmdArgs, '-cleanTestPrefs');
    push(@cmdArgs, '-runTests');
    if (not $options->{disableBatchMode})
    {
        push(@cmdArgs, "-batchmode");
    }
    push(@cmdArgs, "-projectpath $projectPath");
    push(@cmdArgs, "-testPlatform $platform");
    if (not($platform eq "playmode" or $platform eq "editmode"))
    {
        push(@cmdArgs, "-buildTarget $platform");
    }

    push(@cmdArgs, "-accept-apiupdate");

    my $useLegacyScriptingRuntimeVersion = $this->getOptionValue('use-legacy-scripting-runtime-version');
    my $useLatestScriptingRuntimeVersion = $this->getOptionValue('use-latest-scripting-runtime-version');
    my $apiProfile = $this->getOptionValue('api-profile');

    if (defined($useLegacyScriptingRuntimeVersion))
    {
        push(@cmdArgs, "-scripting-runtime-version legacy");
    }
    else
    {
        push(@cmdArgs, "-scripting-runtime-version latest");
    }

    if (defined($apiProfile))
    {
        push(@cmdArgs, "-api-profile $apiProfile");
    }
    else
    {
        # If no api profile is specified, pass in the default profile based on scripting runtime version
        if (defined($useLegacyScriptingRuntimeVersion))
        {
            push(@cmdArgs, "-api-profile NET_2_0_Subset");
        }
        else
        {
            push(@cmdArgs, "-api-profile NET_Standard_2_0");
        }
    }

    my $testFilter = $this->getOptionValue('testfilter');
    if (defined($testFilter))
    {
        push(@cmdArgs, "-testFilter $testFilter");
    }

    push(@cmdArgs, "-testResults $testresultsxml");

    my $categories = $this->getOptionValue('category');
    if (defined($categories))
    {
        foreach my $c (@{$categories})
        {
            push(@cmdArgs, "-testCategory $c");
        }
    }

    my $logFile = $this->getLogFileName($projectName);
    push(@cmdArgs, "-logFile $logFile");
    push(@cmdArgs, "-forgetProjectPath");
    push(@cmdArgs, "-automated");

    my $testSettingsFilePath = $this->generateTestSettingsFileFromOptions();
    push(@cmdArgs, "-testSettingsFile $testSettingsFilePath");

    if ($options->{enableAllModules})
    {
        push(@cmdArgs, "-enableAllModules");
    }

    push(@cmdArgs, "-upmNoDefaultPackages");
    push(@cmdArgs, $this->getExtraArguments($projectName));

    Logger::minimal("Running tests for project $projectName");
    Logger::minimal("Log file:");
    Logger::minimal("\t$logFile");

    my $runnerExitCode = $this->executeAndWatchFiles(
        command => \@cmdArgs,
        filesToWatch => [{ file => $logFile, callback => \&SuiteRunner::watchTestProtocolMessages, callbackArg => $this }]
    );

    push(@{ $this->{testresults} }, $testresultsxml);
    $this->{$testresultsxml} = { projectPath => $projectPath, exitCode => $runnerExitCode };
    return $runnerExitCode;
}

sub getExtraArguments
{
    return ();
}

sub resolveProjects
{
    my ($this) = @_;

    my $projectListFileName = resolveProjectListPath($this->getOptionValue('projectlist'));
    my @projectDirs = $this->_getProjectDirs($projectListFileName);
    my $projectNames = $this->getOptionValue('testprojects');
    if (not defined($projectNames))
    {
        return _resolvePaths(@projectDirs);
    }

    my @result;
    foreach my $prj (@{$projectNames})
    {
        foreach my $dir (@projectDirs)
        {
            if (-d "$dir/Assets")
            {
                if (grep { $_ =~ getLastFolder($dir) } @{$projectNames})
                {
                    push(@result, canonpath($dir));
                }
            }
            else
            {
                push(@result, _resolvePath(catdir($dir, $prj)));
            }
        }
    }

    return uniq(@result);
}

sub onMessage
{
    my ($this, $msg, $filename) = @_;

    if (isTestBegin($msg))
    {
        $this->processTestStartMessage($msg);
    }
    elsif (isTestEnd($msg))
    {
        $this->processTestFinishMessage($msg);
    }
    elsif (isTestPlan($msg))
    {
        $this->processTestPlanMessage($msg);
    }
}

my $totalNumberOfTests : shared = undef;
my $currentTestNumber : shared = 0;

sub processTestPlanMessage
{
    my ($this, $msg) = @_;
    my $testPlan = TestPlan::getTestPlanFromTestProtocolMessage($msg);
    $totalNumberOfTests = $testPlan->getTotalTestCases();
    $currentTestNumber = 0;

    if ($this->getProgressPresenter()->can("setDescriptionWidth"))
    {
        my $maxNameLength = 0;
        foreach my $testName ($testPlan->getTestNames())
        {
            if ($maxNameLength < length($testName))
            {
                $maxNameLength = length($testName);
            }
        }
        $this->getProgressPresenter()->setDescriptionWidth($maxNameLength);
    }
}

sub processTestStartMessage
{
    my ($this, $msg) = @_;
    ++$currentTestNumber;
    my $progressPresenter = $this->getProgressPresenter();
    $progressPresenter->stepStarted(
        progress => { current => $currentTestNumber, total => $totalNumberOfTests },
        progressDetails => $msg->{name}
    );
}

sub processTestFinishMessage
{
    my ($this, $msg) = @_;
    my $progressPresenter = $this->getProgressPresenter();
    my $state = TestResult::stateToString($msg->{state});
    my $duration = $msg->{duration};

    $progressPresenter->stepDone(
        progress => { current => $currentTestNumber, total => $totalNumberOfTests },
        progressDetails => "$msg->{name} $state",
        description => 'test',
        duration => $duration,
        state => $msg->{state}
    );

    $this->SUPER::processTestFinishMessage($msg);
}

sub _resolvePaths
{
    my (@paths) = @_;

    my @results;
    foreach my $path (@paths)
    {
        push(@results, _resolvePath($path));
    }

    return @results;
}

sub _resolvePath
{
    my ($path) = @_;
    my @results;
    if (not -d $path)
    {
        return @results;
    }
    finddepth(
        {
            wanted => sub
            {
                my $d = $File::Find::dir;
                if ($d =~ /[\\\/]Assets$/)
                {
                    $d =~ s/[\\\/]Assets$//;
                    push(@results, canonpath($d));
                }
            },
            no_chdir => 0,
            bydepth => 1
        },
        $path
    );
    @results = uniq(@results);
    return sort (@results);
}

sub _getProjectDirs
{
    my ($this, $projectListFileName) = @_;
    if ($this->{paths})
    {
        return @{ $this->{paths} };
    }

    Logger::minimal("Reading project list from: $projectListFileName");

    my @lines = FileUtils::readAllLines($projectListFileName);
    @lines = grep(!/^#/, @lines);
    @lines = grep(!/^\s*$/, @lines);
    my @allPaths = map { canonpath("$root/$_") } @lines;
    my @results;
    foreach my $path (@allPaths)
    {
        if (not -d $path)
        {
            Logger::warning("$projectListFileName contains invalid path: '$path'");
            next;
        }
        push(@results, $path);
    }
    $this->{paths} = \@results;
    return @results;
}

sub testResultXmlFileName
{
    my ($this, $projectName) = @_;
    my $parentType = getParentType("Unity", $projectName);
    return $this->testResultXmlFileNameImpl($parentType);
}

sub getLogFileName
{
    my ($this, $projectName) = @_;
    return $this->{artifactsNameBuilder}->getFullAtrifactFileName(
        'parent-type' => getParentType("Unity", $projectName),
        type => 'UnityLog',
        extension => 'txt'
    );
}

sub getParentType
{
    my ($parent, $project) = @_;
    my $parentType = $parent;
    if ($project)
    {
        $parentType = "$project/Unity";
    }
    return $parentType;
}

sub startNewIteration
{
    my ($this) = @_;
    $this->{artifact_folders_prepared} = 0;
}

sub processResults
{
    my ($this) = @_;
    foreach my $trxml (@{ $this->{testresults} })
    {
        my $suiteResult = $this->createSuiteResult($trxml);
        my $projectInfo = $this->{$trxml};
        my $projectPath = $projectInfo->{projectPath};
        my $exitCode = $projectInfo->{exitCode};
        if ($exitCode != RETURN_CODE_TESTS_OK and $exitCode != RETURN_CODE_NO_TESTS_FOUND)
        {
            $suiteResult->forceFailed("Suite failed with exit code $exitCode");
        }

        my $fixtureName = FileUtils::removeSubPath($projectPath, Dirs::getRoot());
        $fixtureName =~ s/\\/\//g;
        my @testResults = $suiteResult->getAllResults();
        foreach my $tr (@testResults)
        {
            $tr->setFixture($fixtureName);
        }

        my $projectName = getLastFolder($projectPath);
        $suiteResult->addArtifact($trxml);
        $suiteResult->addArtifact($this->getLogFileName($projectName));
        my $projectNames = $this->getOptionValue('testprojects');
        if (not defined($projectNames))
        {
            $suiteResult->addOptionOverride(name => 'testprojects', value => $projectName);
        }
        $suiteResult->setTestResultXmlFilePath(undef);
        $suiteResult->setDescription($projectName);
        $this->writeSuiteData($suiteResult, $projectName);
        $this->sendResultToConsumer($suiteResult);
    }
}

sub writeSuiteData
{

    #do nothing
}

sub getLastFolder
{
    my ($dirPath) = @_;
    my (@dirs) = splitdir($dirPath);
    return $dirs[-1];
}

sub registerStaticArtifacts
{
}

sub getProjectOptions
{
    my ($path) = @_;
    my %options;
    my $optionsFile = catfile($path, 'TestRunnerOptions.json');
    if (not -e $optionsFile)
    {
        $options{enableAllModules} = 1;
        return \%options;
    }
    my $text = join(q (), FileUtils::readAllLines($optionsFile));
    my $res = jsonToObj($text);
    if (not defined($res->{enableAllModules}))
    {
        $res->{enableAllModules} = 1;
    }
    return $res;
}

sub projectHasTestsForPlatform
{
    my ($path, $platform) = @_;

    if (lc $platform eq "editmode")
    {
        $platform = "Editor";
    }

    return
           testForManifest($path)
        || testForEditorFolder($path, $platform)
        || assemblyDefsInDirAllowsPlatform($path, $platform)
        || testForPlaymodeEnabledFlag($path, $platform);
}

sub testForEditorFolder
{
    my ($projPath, $platform) = @_;

    if (lc $platform ne "editor")
    {
        return 0;
    }

    my $result = 0;
    find(
        {
            wanted => sub
            {
                if (lc $_ eq "editor")
                {
                    $result = 1;
                }
            }
        },
        "$projPath/Assets"
    );

    return $result;
}

sub testForPlaymodeEnabledFlag
{
    my ($projPath, $platform) = @_;

    if (lc $platform eq "editor")
    {
        return 0;
    }

    my $projectSettingsFilePath = catfile($projPath, "/ProjectSettings/ProjectSettings.asset");

    if (!(-f $projectSettingsFilePath))
    {
        return 0;
    }

    open(my $fh, '<', $projectSettingsFilePath);

    while (<$fh>)
    {
        my $line = $_;
        if (index($line, "playModeTestRunnerEnabled: 1") != -1)
        {
            close($fh);
            return 1;
        }
    }

    close($fh);
    return 0;
}

sub testForManifest
{
    my ($projPath) = @_;
    my $manifestPath = $projPath . "/Packages/manifest.json";

    if (!(-f $manifestPath))
    {
        return 0;
    }

    my $manifestText = join(q (), FileUtils::readAllLines($manifestPath));
    my $manifest = jsonToObj($manifestText);
    my $testables = $manifest->{testables};

    if ($testables and scalar $testables > 0)
    {
        return 1;
    }

    return 0;
}

sub assemblyDefsInDirAllowsPlatform
{
    my ($dirPath, $platform) = @_;

    opendir(my $dir, $dirPath);
    my @files = readdir $dir;
    closedir $dir;

    foreach my $file (@files)
    {
        my $fullPath = catfile($dirPath, $file);
        if ($file ne "." and $file ne ".." and -d $fullPath)
        {
            if (assemblyDefsInDirAllowsPlatform($fullPath, $platform) eq 1)
            {
                return 1;
            }
        }
        else
        {
            my ($name, $dir, $ext) = fileparse($file, ".asmdef");
            if ($ext)
            {
                return assemblyDefTestAssemblyAllowsPlatform($fullPath, $platform);
            }
        }
    }

    return 0;
}

sub assemblyDefTestAssemblyAllowsPlatform
{
    my ($assemblyDefPath, $platform) = @_;
    my $assemblyDefText = join(q (), FileUtils::readAllLines($assemblyDefPath));
    my $assemblyDef = jsonToObj($assemblyDefText);
    my @optionalUnityReferences;

    if ($assemblyDef->{optionalUnityReferences})
    {
        @optionalUnityReferences = @{ $assemblyDef->{optionalUnityReferences} };
    }

    if (!@optionalUnityReferences)
    {
        return 0;
    }

    my $testAssembliesReferenced = 0;

    foreach my $ref (@optionalUnityReferences)
    {
        if ($ref eq "TestAssemblies")
        {
            $testAssembliesReferenced = 1;
        }
    }

    if ($testAssembliesReferenced eq 0)
    {
        return 0;
    }

    my @includedPlatforms = @{ $assemblyDef->{includePlatforms} };
    my @excludedPlatforms = @{ $assemblyDef->{excludePlatforms} };

    if (scalar @includedPlatforms eq 0)
    {
        # Looking at the exclude list.
        foreach my $excl (@excludedPlatforms)
        {
            if (lc $excl eq lc $platform)
            {
                return 0;
            }
        }

        return 1;
    }

    # Looking at the include list
    my $playmodeIncluded = 0;
    my $editmodeIncluded = 0;
    foreach my $incl (@includedPlatforms)
    {
        if (lc($incl) eq lc($platform))
        {
            return 1;
        }
    }

    return 0;
}

sub parseResults
{
    my ($this, $testresultsxml) = @_;
    return $this->parseTestResultsXml($testresultsxml);
}

sub prepareArtifactsFolders
{
    my ($this) = @_;
    Logger::minimal("preparing artifacts folder");
    if ($this->{artifact_folders_prepared})
    {
        return;
    }
    my $artifactsPath = $this->getArtifactsPath();
    if (not -d $artifactsPath)
    {
        my $res = mkpath($artifactsPath);
        if (not $res)
        {
            croak("Can not create dir: $artifactsPath");
        }
    }
    my @projectPaths = $this->resolveProjects();

    for my $proj (@projectPaths)
    {
        my $projectName = getLastFolder($proj);
        $this->createArtifactsDirIfNotExistsOrCroak("$projectName/Unity");
        if ($this->getName() =~ /^performance/)
        {
            $this->createArtifactsDirIfNotExistsOrCroak("$projectName/Unity.PerformanceTests.Reporter");
        }
    }

    $this->createArtifactsDirIfNotExistsOrCroak('UnifiedTestRunner');

    $this->{artifact_folders_prepared} = 1;
}

#example: Tests/EditorTests/projectlist.txt --> c:\unity\Tests\EditorTests\projectlist.txt
sub resolveProjectListPath
{
    my ($path) = @_;
    if (-e $path)
    {
        return $path;
    }
    my $result = rel2abs($path, $root);
    return $result;
}

sub _processProjectNotRevoledError
{
    my ($this) = @_;
    my $projectNames = $this->getOptionValue('testprojects');
    my $msg;
    if ($projectNames and scalar(@{$projectNames}) > 0)
    {
        $msg = "Can not resolve any project paths matching: " . join(' ', @{$projectNames});
    }
    else
    {
        $msg = "Can not resolve any project paths";
    }
    Logger::error($msg);
    $this->{runnerExitCode} = RETURN_CODE_FAILED_TO_RESOLVE_PROJECT_PATH;
}

sub _reportPerformanceData
{
    my ($this, $projectName) = @_;
    PerformanceReporter::report(
        $this->getEnvironment(),
        suiteName => $this->getReporterSuiteName(),
        reporterPath => $this->getPathToReporter(),
        platform => $this->getPlatform(),
        testResultsXml => $this->testResultXmlFileName($projectName),
        testResultsJson => $this->testResultJsonFileName($projectName),
        outputJson => $this->performanceDataFileName(),
        csvResultsFile => $this->getOptionValue('testresultscsv'),
        stdOut => $this->reporterStdOutFileName(),
        stdErr => $this->reporterStdErrFileName(),
    );
}

sub getPlatformTestSettings
{
    my ($this, $platform, $action) = @_;
    my $target = BuildTargetResolver::resolveTarget($platform);
    if (not defined($target))
    {
        return;
    }
    return $target->getTestSettings();
}

sub generateTestSettingsFileFromOptions
{
    my ($this) = @_;
    my %testSettings;

    my $platform = $this->getOptionValue('platform');

    my $scriptingBackend = $this->getOptionValue('scripting-backend');
    my $useLegacyScriptingRuntimeVersion = $this->getOptionValue('use-legacy-scripting-runtime-version');
    my $useLatestScriptingRuntimeVersion = $this->getOptionValue('use-latest-scripting-runtime-version');
    my $apiProfile = $this->getOptionValue('api-profile');

    if (defined($useLegacyScriptingRuntimeVersion))
    {
        $testSettings{'useLatestScriptingRuntimeVersion'} = 'false';
    }
    else
    {
        $testSettings{'useLatestScriptingRuntimeVersion'} = 'true';

        #Consoles on latest scripting runtime require IL2CPP scripting backend
        if (lc $platform eq 'ps4')
        {
            $scriptingBackend = 'IL2CPP';
        }
    }

    if (defined($apiProfile))
    {
        $testSettings{'apiProfile'} = $apiProfile;
    }
    else
    {
        # If no api profile is specified, pass in the default profile based on scripting runtime version
        if (defined($useLegacyScriptingRuntimeVersion))
        {
            $testSettings{'apiProfile'} = 'NET_2_0_Subset';
        }
        else
        {
            $testSettings{'apiProfile'} = 'NET_Standard_2_0';
        }
    }

    $testSettings{'scriptingBackend'} = $scriptingBackend;

    my $architecture = $this->getOptionValue('architecture');
    if (defined($architecture))
    {
        $testSettings{'architecture'} = $architecture;
    }

    my $platformSettings = $this->getPlatformTestSettings($platform);
    if ($platformSettings)
    {
        %testSettings = (%testSettings, %{$platformSettings});
    }

    my $json = JSON->new(utf8 => 1, convblessed => 1);
    my $jsonText = $json->to_json(\%testSettings);
    my $path = new File::Temp() . '_TestSettings.json';
    FileUtils::writeAllLines($path, $jsonText);

    return $path;
}

sub afterProjectRun
{
    # do nothing
}

1;
