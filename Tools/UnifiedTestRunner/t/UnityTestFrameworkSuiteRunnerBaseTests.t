use warnings;
use strict;

use Test::More;

use File::Temp qw (tempdir);
use File::Path qw (remove_tree make_path);
use File::Spec::Functions qw (catfile catdir canonpath);

use TestHelpers::Runners qw (runAndGrabExecuteInput makeDefaultSuiteRunnerArtifactNameBuilderStub);

use Dirs;
use lib Dirs::external_root();
use Test::MockObject;
use Test::MockObject::Extends;
use RunContext;
use TestHelpers::EditorPlaymode qw (createEmptyUnityProject createPlaymodeUnityProject createEditmodeUnityProject createPlaymodeAndEditmodeUnityProject);
use TestHelpers::ArrayAssertions qw (isEmpty arrayDoesNotContain arrayStartsWith arrayContains);
use Test::Deep 're';
use ResultCollector;
use File::Copy;

my $testDataFolder;

BEGIN
{
    use_ok('UnityTestFrameworkSuiteRunnerBase');
    $testDataFolder = tempdir();
}

my $suiteOptions = [
    ['projectlist:s', 'Project list filename', { defaultValue => 'default_list' }],
    ['testprojects:s@'],
    ['category:s@'],
    [
        'platform:s',
        {
            defaultValue => 'default_platform_value'
        }
    ],
    [
        'scripting-backend:s',
        'Scripting backend to execute test on.',
        {
            defaultValue => 'Mono2x',
            allowedValues => ['Mono2x', 'IL2CPP', 'WinRTDotNET']
        }
    ],
    ['use-latest-scripting-runtime-version', 'Whether to use the latest scripting runtime version or not.'],
    ['assetpipelinev2', 'Enable Asset Import Pipeline V2 backend for the test run.']
];

DefaultValues:
{
    my $r = createRunner();
    is($r->getOptionValue('platform'), 'default_platform_value');
    is($r->getOptionValue('projectlist'), 'default_list');
    is($r->getName(), 'RunnerName');
    is($r->getPlatform(), $r->getName());
}

Run_DefaultArgs_ProduceCorrectCommandLine:
{
    my $runner = createRunner();

    my $nameBuilderStub = makeDefaultSuiteRunnerArtifactNameBuilderStub();

    $nameBuilderStub->specifyExpectation(
        method => 'getFullAtrifactFileName',
        params => { 'parent-type' => 'DefaultProject/Unity', type => 'TestResults', extension => 'xml' },
        result => 'test_results_xml_full_file_name'
    );

    $nameBuilderStub->specifyExpectation(
        method => 'getFullAtrifactFileName',
        params => { 'parent-type' => 'DefaultProject/Unity', type => 'UnityLog', extension => 'txt' },
        result => 'full_path_to_unity_log_file'
    );

    $runner->setArtifactsNameBuilder($nameBuilderStub->object());
    my $projectPath = createPlaymodeUnityProject($testDataFolder, "DefaultProject");
    $runner->mock(
        '_getProjectDirs' => sub
        {
            return ($projectPath);
        }
    );
    my @input = TestHelpers::Runners::runAndGrabExecuteInput($runner);

    arrayContains(
        \@input,
        [
            re(qr/Unity/i), '-runTests',
            '-batchmode', "-projectpath $projectPath",
            '-cleanTestPrefs', '-testPlatform default_platform_value',
            '-buildTarget default_platform_value', '-accept-apiupdate',
            '-scripting-runtime-version latest', '-api-profile NET_Standard_2_0',
            '-testResults test_results_xml_full_file_name', '-logFile full_path_to_unity_log_file',
            '-upmNoDefaultPackages', '-enableAllModules',
        ]
    );
}

Run_TestProjectIsSpecified_ProducesCorrectCommandLine:
{
    my $projectPath = createPlaymodeUnityProject($testDataFolder, "Project1");
    my $runner = createRunner();
    my @input = runAndGrabExecuteInput($runner, '--testprojects=Project1');
    arrayContains(\@input, "-projectpath $projectPath");
}

Run_Platform_ProduceCorrectCommandLine:
{
    my @input = run("--platform=playmode");
    arrayContains(\@input, "-testPlatform playmode");
}

Run_TestFilterIsSpecified_ProducesCorrectCommandLine:
{
    my @input = run('--testfilter=sometestname');
    arrayContains(\@input, "-testFilter sometestname");
}

Run_OneTestCategoryIsSpecified_ProducesCorrectCommandLine:
{
    my @input = run('--category=ABV');
    arrayContains(\@input, "-testCategory ABV");
}

Run_TwoTestCategoriesAreSpecified_ProducesCorrectCommandLine:
{
    my @input = run('--category=ABV', '--category=BVA');
    arrayContains(\@input, ['-testCategory ABV', '-testCategory BVA']);
}

Run_NonExistingProject_DoesNotRuneAnything:
{
    my $runner = createRunner();
    my @input = runAndGrabExecuteInput($runner, '--testproject=does_not_exists');
    isEmpty(\@input);
    is($runner->{runnerExitCode}, UnityTestFrameworkSuiteRunnerBase::RETURN_CODE_FAILED_TO_RESOLVE_PROJECT_PATH);
}

Run_BatchmodeOption_AddsBatchmodeFlagToTheCommandLine:
{
    my $projectPath = createEmptyUnityProject($testDataFolder, "BatchTest");
    my $runnerConfigFile = catfile($projectPath, "TestRunnerOptions.json");
    FileUtils::writeAllLines($runnerConfigFile, '{"disableBatchMode":true}"');
    my @input = run("--testprojects=BatchTest");
    arrayDoesNotContain(\@input, ['-batchmode']);
}

Run_DisabledProject_DoesNotRun:
{
    my $projectPath = createEmptyUnityProject($testDataFolder, "DisableTest");
    my $runnerConfigFile = catfile($projectPath, "TestRunnerOptions.json");
    FileUtils::writeAllLines($runnerConfigFile, '{"disabled":true}"');
    my @input = run("--testprojects=DisableTest");
    isEmpty(\@input);
}

Run_DisabledProjectOnLatestScriptingRuntime_DoesNotRun:
{
    my $projectPath = createEmptyUnityProject($testDataFolder, "DisableTestOnLatestScriptingRuntime");
    my $runnerConfigFile = catfile($projectPath, "TestRunnerOptions.json");
    FileUtils::writeAllLines($runnerConfigFile, '{"disabledOnLatestScriptingRuntime":true}"');
    my @input = run('--testprojects=DisableTestOnLatestScriptingRuntime', '--use-latest-scripting-runtime-version');
    isEmpty(\@input);
}

Run_DisabledProjectOnLegacyScriptingRuntime_DoesNotRun:
{
    my $projectPath = createEmptyUnityProject($testDataFolder, "DisableTestOnLegacyScriptingRuntime");
    my $runnerConfigFile = catfile($projectPath, "TestRunnerOptions.json");
    FileUtils::writeAllLines($runnerConfigFile, '{"disabledOnLegacyScriptingRuntime":true}"');
    my @input = run('--testprojects=DisableTestOnLegacyScriptingRuntime');
    isEmpty(\@input);
}

Run_DisabledProjectOnIL2CPPScriptingBackend_DoesNotRun:
{
    my $projectPath = createEmptyUnityProject($testDataFolder, "DisableTestOnIL2CPPScriptingBackend");
    my $runnerConfigFile = catfile($projectPath, "TestRunnerOptions.json");
    FileUtils::writeAllLines($runnerConfigFile, '{"disabledOnIL2CPPScriptingBackend":true}"');
    my @input = run('--testprojects=DisableTestOnIL2CPPScriptingBackend', '--scripting-backend=IL2CPP');
    isEmpty(\@input);
}

Run_AssetPipelineV2EnabledProject_RunsInV2Mode:
{
    my $projectPath = createPlaymodeUnityProject($testDataFolder, "V2EnabledTestV2Mode");
    my $runnerConfigFile = catfile($projectPath, "TestRunnerOptions.json");
    FileUtils::writeAllLines($runnerConfigFile, '{"allowAssetPipelineV2":true}"');

    my $runner = createRunner();
    $runner = TestHelpers::Runners::makeRunnerMock($runner);
    my @input = runAndGrabExecuteInput($runner, ('--testprojects=V2EnabledTestV2Mode', '--assetpipelinev2'));

    arrayContains(\@input, "-assetpipelinev2");
    arrayContains(\@input, "-ImportModeForced");
}

Run_AssetPipelineV2EnabledProject_RunsInV1Mode:
{
    my $projectPath = createPlaymodeUnityProject($testDataFolder, "V2EnabledTestV1Mode");
    my $runnerConfigFile = catfile($projectPath, "TestRunnerOptions.json");
    FileUtils::writeAllLines($runnerConfigFile, '{"allowAssetPipelineV2":true}"');

    my @input = run('--testprojects=V2EnabledTestV1Mode');

    ok(scalar(@input) > 0);
    arrayDoesNotContain(\@input, "-assetpipelinev2");
}

Run_AssetPipelineV2DisabledProject_IsSkippedInV2Mode:
{
    my $projectPath = createEmptyUnityProject($testDataFolder, "V2DisabledTestV2Mode");
    my $precompileWasCalled;

    my $runner = createRunner();
    $runner = TestHelpers::Runners::makeRunnerMock($runner);
    $runner->mock(
        'precompileExtensionsLocally' => sub
        {
            $precompileWasCalled = 1;
            return undef;
        }
    );
    my @input = runAndGrabExecuteInput($runner, ('--testprojects=V2DisabledTest', '--assetpipelinev2'));

    is($precompileWasCalled, undef);
    isEmpty(\@input);
}

RunOnlyInEditorJsonOption:
{
    my $projectPath = createPlaymodeAndEditmodeUnityProject($testDataFolder, "EditorOnlyProject");
    my $runnerConfigFile = catfile($projectPath, "TestRunnerOptions.json");
    FileUtils::writeAllLines($runnerConfigFile, '{"runOnlyInEditor":true}');
    make_path("$testDataFolder/EditorOnlyProject/Assets/editor");
    FileUtils::writeAllLines("$testDataFolder/EditorOnlyProject/Assets/PlaymodeTest.cs", '');

    my @input = run('--testprojects=EditorOnlyProject', '--platform=editmode');
    ok(scalar(@input) > 0);

    @input = run('--testprojects=EditorOnlyProject', '--platform=playmode');
    ok(scalar(@input) > 0);

    @input = run('--testprojects=EditorOnlyProject', '--platform=ios');
    ok(scalar(@input) == 0);
}

AllowedPlatformsJsonOption:
{
    my $projectPath = createPlaymodeAndEditmodeUnityProject($testDataFolder, "AllowedPlatformProject");
    my $runnerConfigFile = catfile($projectPath, "TestRunnerOptions.json");
    FileUtils::writeAllLines($runnerConfigFile, '{"allowedPlatforms":["iOS", "Android"]}');
    FileUtils::writeAllLines("$testDataFolder/AllowedPlatformProject/Assets/PlaymodeTest.cs", '');

    my @input = run('--testprojects=AllowedPlatformProject', '--platform=ios');
    ok(scalar(@input) > 0);

    @input = run('--testprojects=AllowedPlatformProject', '--platform=android');
    ok(scalar(@input) > 0);

    @input = run('--testprojects=AllowedPlatformProject', '--platform=Editor');
    ok(scalar(@input) == 0);
}

Run_ProjectWithEditorOnly_SkipsTheProjectDependingOnThePlatform:
{
    my $projectName = "EditorOnlyProject2";
    my $projectPath = createEmptyUnityProject($testDataFolder, $projectName);
    make_path("$testDataFolder/$projectName/Assets/editor");
    FileUtils::writeAllLines("$testDataFolder/$projectName/Assets/editor/EditModeTest.cs", '');

    my @input = run("--testprojects=$projectName", "--platform=editmode");
    ok(scalar(@input) > 0);

    @input = run("--testprojects=$projectName", "--platform=playmode");
    ok(scalar(@input) == 0);

    @input = run("--testprojects=$projectName", "--platform=ios");
    ok(scalar(@input) == 0);
}

Run_ProjectWithPlayerAndNestedEditorScripts_RunsBothTypesOfSuites:
{
    my $projectName = "EditorOnlyProject2";
    my $projectPath = createPlaymodeUnityProject($testDataFolder, $projectName);

    make_path("$testDataFolder/$projectName/Assets/Tests/Editor");
    FileUtils::writeAllLines("$testDataFolder/$projectName/Assets/Tests/Editor/EditModeTest.cs", '');

    my @input = run("--testprojects=$projectName", "--platform=editmode");
    ok(scalar(@input) > 0);

    @input = run("--testprojects=$projectName", "--platform=playmode");
    ok(scalar(@input) > 0);
}

Run_ProjectWithPlaymodeOnly_SkipsTheProjectDependingOnThePlatform:
{
    my $projectName = "PlayModeOnlyProject";
    my $projectPath = createPlaymodeUnityProject($testDataFolder, $projectName);

    my @input = run("--testprojects=$projectName", "--platform=editmode");
    ok(scalar(@input) == 0);

    @input = run("--testprojects=$projectName", "--platform=playmode");
    ok(scalar(@input) > 0);

    @input = run("--testprojects=$projectName", "--platform=ios");
    ok(scalar(@input) > 0);
}

Run_FolderWithTwoNestedFolder_ProducesCorrectCommandLine:
{
    my $projectsRoot = catdir($testDataFolder, "FolderWith2NestedProjects");
    make_path($projectsRoot);

    my $projectPath1 = createPlaymodeUnityProject($projectsRoot, "Project1");
    my $projectPath2 = createPlaymodeUnityProject($projectsRoot, "Project2");

    my $runner = createRunner();
    $runner = new Test::MockObject::Extends($runner);
    my @calls;
    $runner->mock(
        executeAndWatchFiles => sub
        {
            my ($this, %args) = @_;
            push(@calls, $args{command});
        }
    );

    TestHelpers::Runners::runAndGrabExecuteInput($runner, "--testprojects=FolderWith2NestedProjects");
    my $expectedTestResultsXmlName = canonpath("Project1/Unity/TestResults.xml");
    arrayStartsWith(
        $calls[0],
        [
            re(qr/Unity/i), '-cleanTestPrefs',
            '-runTests', '-batchmode',
            "-projectpath $projectPath1", '-testPlatform default_platform_value',
            '-buildTarget default_platform_value', '-accept-apiupdate',
            '-scripting-runtime-version latest', '-api-profile NET_Standard_2_0',
            re(qr /-testResults .*Project1[\/\\]Unity[\/\\]TestResults.xml$/), re(qr /-logFile.*Project1[\/\\]Unity[\/\\]UnityLog.txt$/),
        ]
    );

    arrayStartsWith(
        $calls[1],
        [
            re(qr/Unity/i), '-cleanTestPrefs',
            '-runTests', '-batchmode',
            "-projectpath $projectPath2", '-testPlatform default_platform_value',
            '-buildTarget default_platform_value', '-accept-apiupdate',
            '-scripting-runtime-version latest', '-api-profile NET_Standard_2_0',
            re(qr /-testResults.*Project2[\/\\]Unity[\/\\]TestResults.xml$/), re(qr /-logFile.*Project2[\/\\]Unity[\/\\]UnityLog.txt$/),
        ]
    );
}

Run_TwoProjects_Report2SuiteResultsWithCorrectArtifacts:
{
    my $projectsRoot = catdir($testDataFolder, 'Run_TwoProjects_Report2SuiteResultsWithCorrectArtifacts');
    my $projectPath1 = createPlaymodeUnityProject($projectsRoot, "Project1");
    my $projectPath2 = createPlaymodeUnityProject($projectsRoot, "Project2");

    my $runner = createRunner();
    $runner = new Test::MockObject::Extends($runner);

    my $resultConsumer = new ResultCollector();
    $runner->setResultConsumer($resultConsumer);
    runAndGrabExecuteInput($runner, "--testprojects=Run_TwoProjects_Report2SuiteResultsWithCorrectArtifacts");
    my @suiteResults = $resultConsumer->getAllSuiteResults();
    my @sr1Artifacts = $suiteResults[0]->getArtifacts();
    my @sr2Artifacts = $suiteResults[1]->getArtifacts();

    is(scalar(@sr1Artifacts), 2);
    is(scalar(@sr2Artifacts), 2);
    arrayContains(\@sr1Artifacts, [re(qr /.*Project1[\/\\]Unity[\/\\]TestResults.xml$/), re(qr /.*Project1[\/\\]Unity[\/\\]UnityLog.txt$/),]);

    arrayContains(\@sr2Artifacts, [re(qr /.*Project2[\/\\]Unity[\/\\]TestResults.xml$/), re(qr /.*Project2[\/\\]Unity[\/\\]UnityLog.txt$/)]);
}

Run_TestProjectIsSpecified_ProducesCorrectCommandLine:
{
    my $projectsRoot1 = tempdir();
    my $projectPath1 = createPlaymodeUnityProject($projectsRoot1, "ProjectInRoot1");
    my $projectsRoot2 = tempdir();
    my $projectPath2 = createPlaymodeUnityProject($projectsRoot2, "ProjectInRoot2");

    my $runner = createRunner();

    $runner->mock(
        '_getProjectDirs' => sub
        {
            return ($projectsRoot1, $projectsRoot2);
        }
    );

    my @calls;
    $runner->mock(
        executeAndWatchFiles => sub
        {
            my ($this, %args) = @_;
            push(@calls, $args{command});
        }
    );

    my @input = runAndGrabExecuteInput($runner);
    arrayContains($calls[0], "-projectpath $projectPath1");
    arrayContains($calls[1], "-projectpath $projectPath2");
    remove_tree($projectsRoot1);
    remove_tree($projectsRoot2);
}

Run_DoesNotTreatFolderWithAssetsSuffixAsUnityProject:
{
    my $projectsRoot = tempdir();
    my $projectPath = createEmptyUnityProject($projectsRoot);
    ok(-d "$projectPath/Assets");
    move("$projectPath/Assets", "$projectPath/TestAssets");
    ok(-d "$projectPath/TestAssets");
    my $runner = createRunner();

    $runner->mock(
        '_getProjectDirs' => sub
        {
            return ($projectsRoot);
        }
    );

    my @input = runAndGrabExecuteInput($runner);
    isEmpty(\@input);
    remove_tree($projectsRoot);
}

Run_CanOverrideProjectList:
{
    my $projectsRoot = catdir($testDataFolder, 'ProjectListTest');

    my $projectPath1 = createPlaymodeUnityProject($projectsRoot, "Project1");
    my $projectPath2 = createPlaymodeUnityProject($projectsRoot, "Project2");
    my $projectListFileName = canonpath("$projectsRoot/projectlist.txt");
    FileUtils::writeAllLines($projectListFileName, 'Project1', 'Project2');

    my $runner = createRunner();
    $runner->mock(
        '_getProjectDirs' => sub
        {
            return $projectsRoot;
        }
    );

    my @calls;
    $runner->mock(
        executeAndWatchFiles => sub
        {
            my ($this, %args) = @_;
            push(@calls, $args{command});
        }
    );

    runAndGrabExecuteInput($runner, "--projectlist=$projectListFileName");
    arrayContains($calls[0], "-projectpath $projectPath1");
    arrayContains($calls[1], "-projectpath $projectPath2");
}

Run_ProjectListContainsPathWhichMatchesPathToProject:
{
    my $projectsRoot = catdir($testDataFolder, 'ProjectListTest');

    my $projectPath1 = createEmptyUnityProject($projectsRoot, "Project1");
    my $projectListFileName = canonpath("$projectsRoot/projectlist.txt");
    FileUtils::writeAllLines($projectListFileName, 'Project1');

    my $runner = createRunner();
    $runner->mock(
        '_getProjectDirs' => sub
        {
            return $projectPath1;
        }
    );

    my @calls;
    $runner->mock(
        executeAndWatchFiles => sub
        {
            my ($this, %args) = @_;
            push(@calls, $args{command});
        }
    );

    runAndGrabExecuteInput($runner, "--projectlist=$projectListFileName", "--testproject=Project1");
    is(scalar(@calls), 1);
    arrayContains($calls[0], "-projectpath $projectPath1");
}

Run_ProjectListContainsPathWhichMatchesPathToProjectAndTestProjectIsSpecified:
{
    my $projectsRoot = catdir($testDataFolder, 'ProjectListTest2');

    my $projectPath1 = createPlaymodeUnityProject($projectsRoot, "Project1");
    my $projectPath2 = createPlaymodeUnityProject($projectsRoot, "Project2");
    my $projectListFileName = canonpath("$projectsRoot/projectlist.txt");
    FileUtils::writeAllLines($projectListFileName, 'Project1');

    my $runner = createRunner();
    $runner->mock(
        '_getProjectDirs' => sub
        {
            return ($projectPath1, $projectPath2);
        }
    );

    my @calls;
    $runner->mock(
        executeAndWatchFiles => sub
        {
            my ($this, %args) = @_;
            push(@calls, $args{command});
        }
    );

    runAndGrabExecuteInput($runner, "--projectlist=$projectListFileName", "--testproject=Project1");
    is(scalar(@calls), 1);
    arrayContains($calls[0], "-projectpath $projectPath1");
}

Run_ProjectListIsSpecifiedAndProjectNameContainsNonExistingProject:
{
    my $projectsRoot = catdir($testDataFolder, 'ProjectListTest3');

    my $projectPath1 = createPlaymodeUnityProject($projectsRoot, "Project1");
    my $projectPath2 = createPlaymodeUnityProject($projectsRoot, "Project2");
    my $projectListFileName = canonpath("$projectsRoot/projectlist.txt");
    FileUtils::writeAllLines($projectListFileName, 'Project1');

    my $runner = createRunner();
    $runner->mock(
        '_getProjectDirs' => sub
        {
            return ($projectPath1, $projectPath2);
        }
    );

    my @calls;
    $runner->mock(
        executeAndWatchFiles => sub
        {
            my ($this, %args) = @_;
            push(@calls, $args{command});
        }
    );

    runAndGrabExecuteInput($runner, "--projectlist=$projectListFileName", "--testproject=Project1", "--testproject=Project3");
    is(scalar(@calls), 1);
    arrayContains($calls[0], "-projectpath $projectPath1");
}

Run_PlatformIsPlaymode_ProducesNoBuildTarget:
{
    my @input = run("--platform=playmode");
    arrayDoesNotContain(\@input, "-buildTarget playmode");
}

Run_PlatformIsEditmode_ProducesNoBuildTarget:
{
    my @input = run("--platform=editmode");
    arrayDoesNotContain(\@input, "-buildTarget editmode");
}

Run_ForgetsProjectPath_ProduceCorrectCommandLine:
{
    my @input = run();
    arrayContains(\@input, "-forgetProjectPath");
}

sub begins_with
{
    return substr($_[0], 0, length($_[1])) eq $_[1];
}

sub assertSettingsJsonValue
{
    my $settingsJson = $_[0];
    my $expectedJson = $_[1];
    ok(index($settingsJson, $expectedJson) != -1, "JsonSettings did not contain expected value.\nSettings: '$settingsJson'\nExpected value: '$expectedJson'.");
}

Run_ProvidedTestSettings_ProducesCommandLineWithCorrectTestSettings:
{
    my $projectPath = createEmptyUnityProject($testDataFolder, "Project1");
    my $runner = createRunner();

    my @input = run('--scripting-backend=IL2CPP', '--use-latest-scripting-runtime-version');

    my $testSettingsFilePath = "";
    foreach $a (@input)
    {
        if (begins_with($a, "-testSettingsFile "))
        {
            $testSettingsFilePath = substr($a, length("-testSettingsFile "));
        }
    }

    ok(length($testSettingsFilePath) > 0);

    my $settingsJson = join('', FileUtils::readAllLines($testSettingsFilePath));
    assertSettingsJsonValue($settingsJson, '"scriptingBackend":"IL2CPP"');
    assertSettingsJsonValue($settingsJson, '"useLatestScriptingRuntimeVersion":true');
    assertSettingsJsonValue($settingsJson, '"apiProfile":"NET_Standard_2_0"');
}

Run_TestModeDetermination_ForEmptyProject:
{
    my $projectName = "EmptyProject";
    my $projectPath = createEmptyUnityProject($testDataFolder, $projectName);
    my $runner = createRunner();

    my @input = run("--testprojects=$projectName", '--platform=editmode');
    ok(scalar(@input) == 0);

    @input = run("--testprojects=$projectName", '--platform=playmode');
    ok(scalar(@input) == 0);

    @input = run("--testprojects=$projectName", '--platform=ios');
    ok(scalar(@input) == 0);
}

Run_TestModeDetermination_ForProjectWithEditorFolder:
{
    my $projectName = "ProjectWithEditorFolder";
    my $projectPath = createEditmodeUnityProject($testDataFolder, $projectName);
    my $runner = createRunner();

    my @input = run("--testprojects=$projectName", '--platform=editmode');
    ok(scalar(@input) > 0);

    @input = run("--testprojects=$projectName", '--platform=playmode');
    ok(scalar(@input) == 0);

    @input = run("--testprojects=$projectName", '--platform=ios');
    ok(scalar(@input) == 0);
}

Run_TestModeDetermination_ForProjectWithEditorSubFolder:
{
    my $projectName = "ProjectWithEditorSubFolder";
    my $projectPath = createEmptyUnityProject($testDataFolder, $projectName);
    make_path("$projectPath/Assets/MyTests/Editor");
    FileUtils::writeAllLines("$projectPath/Assets/MyTests/Editor/AnEditModeTest.cs", '');
    my $runner = createRunner();

    my @input = run("--testprojects=$projectName", '--platform=editmode');
    ok(scalar(@input) > 0);

    @input = run("--testprojects=$projectName", '--platform=playmode');
    ok(scalar(@input) == 0);

    @input = run("--testprojects=$projectName", '--platform=ios');
    ok(scalar(@input) == 0);
}

Run_TestModeDetermination_ForProjectWithCsFileOutsideEditorFolder:
{
    my $projectName = "ProjectWithPlaymodeCsFile";
    my $projectPath = createPlaymodeUnityProject($testDataFolder, $projectName);
    my $runner = createRunner();

    my @input = run("--testprojects=$projectName", '--platform=editmode');
    ok(scalar(@input) == 0);

    @input = run("--testprojects=$projectName", '--platform=playmode');
    ok(scalar(@input) > 0);

    @input = run("--testprojects=$projectName", '--platform=ios');
    ok(scalar(@input) > 0);
}

Run_TestModeDetermination_ForProjectWithCsFileInsideAndOutsideEditorFolder:
{
    my $projectName = "ProjectWithPlaymodeCsFileAndEditorFolder";
    my $projectPath = createPlaymodeAndEditmodeUnityProject($testDataFolder, $projectName);
    my $runner = createRunner();

    my @input = run("--testprojects=$projectName", '--platform=editmode');
    ok(scalar(@input) > 0);

    @input = run("--testprojects=$projectName", '--platform=playmode');
    ok(scalar(@input) > 0);

    @input = run("--testprojects=$projectName", '--platform=ios');
    ok(scalar(@input) > 0);
}

Run_TestModeDetermination_ForProjectWithManifest:
{
    my $projectName = "ProjectWithManifest";
    my $projectPath = createEmptyUnityProject($testDataFolder, $projectName);
    make_path("$projectPath/Packages");
    FileUtils::writeAllLines(
        "$projectPath/Packages/manifest.json", '{
        "testables": ["com.unity.testpackage" ],
        "dependencies":{
            "com.unity.testpackage": "file:C:\\code\\com.unity.testpackage"
        }
    }'
    );

    my $runner = createRunner();

    my @input = run("--testprojects=$projectName", '--platform=editmode');
    ok(scalar(@input) > 0);

    @input = run("--testprojects=$projectName", '--platform=playmode');
    ok(scalar(@input) > 0);

    @input = run("--testprojects=$projectName", '--platform=ios');
    ok(scalar(@input) > 0);
}

Run_TestModeDetermination_ForProjectWithManifestWithoutTestables:
{
    my $projectName = "ProjectWithManifestWithoutTestables";
    my $projectPath = createEmptyUnityProject($testDataFolder, $projectName);
    make_path("$projectPath/Packages");
    FileUtils::writeAllLines(
        "$projectPath/Packages/manifest.json", '{
        "dependencies":{
            "com.unity.testpackage": "file:C:\\code\\com.unity.testpackage"
        }
    }'
    );

    my $runner = createRunner();

    my @input = run("--testprojects=$projectName", '--platform=editmode');
    ok(scalar(@input) == 0);

    @input = run("--testprojects=$projectName", '--platform=playmode');
    ok(scalar(@input) == 0);

    @input = run("--testprojects=$projectName", '--platform=ios');
    ok(scalar(@input) == 0);
}

Run_TestModeDetermination_ForProjectWithEmptyManifest:
{
    my $projectName = "ProjectWithEmptyManifest";
    my $projectPath = createEmptyUnityProject($testDataFolder, $projectName);
    make_path("$projectPath/Packages");
    FileUtils::writeAllLines("$projectPath/Packages/manifest.json", '{"dependencies": {}}"');

    my $runner = createRunner();

    my @input = run("--testprojects=$projectName", '--platform=editmode');
    ok(scalar(@input) == 0);

    @input = run("--testprojects=$projectName", '--platform=playmode');
    ok(scalar(@input) == 0);

    @input = run("--testprojects=$projectName", '--platform=ios');
    ok(scalar(@input) == 0);
}

Run_TestModeDetermination_ForProjectWithEditorAssemblyDef:
{
    my $projectName = "ProjectWithEditorAssemblyDef";
    my $projectPath = createEmptyUnityProject($testDataFolder, $projectName);
    make_path("$projectPath/Assets/MyTests");
    FileUtils::writeAllLines(
        "$projectPath/Assets/MyTests/EditorAssemblyDef.asmdef", '{
        "optionalUnityReferences": [
            "TestAssemblies"
        ],
        "includePlatforms": [
            "Editor"
        ],
        "excludePlatforms": []
        }'
    );

    my $runner = createRunner();

    my @input = run("--testprojects=$projectName", '--platform=editmode');
    ok(scalar(@input) > 0);

    @input = run("--testprojects=$projectName", '--platform=playmode');
    ok(scalar(@input) == 0);

    @input = run("--testprojects=$projectName", '--platform=ios');
    ok(scalar(@input) == 0);
}

Run_TestModeDetermination_ForProjectWithPlaymodeAssemblyDefUsingInclude:
{
    my $projectName = "ProjectWithPlaymodeAssemblyDefUsingInclude";
    my $projectPath = createEmptyUnityProject($testDataFolder, $projectName);
    make_path("$projectPath/Assets/MyTests");
    FileUtils::writeAllLines(
        "$projectPath/Assets/MyTests/EditorAssemblyDef.asmdef", '{
        "optionalUnityReferences": [
            "TestAssemblies"
        ],
        "includePlatforms": [
            "Android"
        ],
        "excludePlatforms": []
        }'
    );

    my $runner = createRunner();

    my @input = run("--testprojects=$projectName", '--platform=editmode');
    ok(scalar(@input) == 0);

    @input = run("--testprojects=$projectName", '--platform=playmode');
    ok(scalar(@input) == 0);

    @input = run("--testprojects=$projectName", '--platform=Android');
    ok(scalar(@input) > 0);
}

Run_TestModeDetermination_ForProjectWithPlaymodeAssemblyDefUsingExclude:
{
    my $projectName = "ProjectWithPlaymodeAssemblyDefUsingExclude";
    my $projectPath = createEmptyUnityProject($testDataFolder, $projectName);
    make_path("$projectPath/Assets/MyTests");
    FileUtils::writeAllLines(
        "$projectPath/Assets/MyTests/EditorAssemblyDef.asmdef", '{
        "optionalUnityReferences": [
            "TestAssemblies"
        ],
        "includePlatforms": [
        ],
        "excludePlatforms": [
            "Editor",
            "Android"
        ]
        }'
    );

    my $runner = createRunner();

    my @input = run("--testprojects=$projectName", '--platform=editmode');
    ok(scalar(@input) == 0);

    @input = run("--testprojects=$projectName", '--platform=playmode');
    ok(scalar(@input) > 0);

    @input = run("--testprojects=$projectName", '--platform=Android');
    ok(scalar(@input) == 0);
}

Run_TestModeDetermination_ForProjectWithBothAssemblyDefUsingInclude:
{
    my $projectName = "ProjectWithBothAssemblyDefUsingInclude";
    my $projectPath = createEmptyUnityProject($testDataFolder, $projectName);
    make_path("$projectPath/Assets/MyTests");
    FileUtils::writeAllLines(
        "$projectPath/Assets/MyTests/EditorAssemblyDef.asmdef", '{
        "optionalUnityReferences": [
            "TestAssemblies"
        ],
        "includePlatforms": [
            "Editor",
            "Android"
        ],
        "excludePlatforms": [
        ]
        }'
    );

    my $runner = createRunner();

    my @input = run("--testprojects=$projectName", '--platform=editmode');
    ok(scalar(@input) > 0);

    @input = run("--testprojects=$projectName", '--platform=playmode');
    ok(scalar(@input) == 0);

    @input = run("--testprojects=$projectName", '--platform=Android');
    ok(scalar(@input) > 0);
}

Run_TestModeDetermination_ForProjectWithBothAssemblyDefUsingExclude:
{
    my $projectName = "ProjectWithBothAssemblyDefUsingExclude";
    my $projectPath = createEmptyUnityProject($testDataFolder, $projectName);
    make_path("$projectPath/Assets/MyTests");
    FileUtils::writeAllLines(
        "$projectPath/Assets/MyTests/AssemblyDef.asmdef", '{
        "optionalUnityReferences": [
            "TestAssemblies"
        ],
        "includePlatforms": [
        ],
        "excludePlatforms": [
            "Android"
        ]
        }'
    );

    my $runner = createRunner();

    my @input = run("--testprojects=$projectName", '--platform=editmode');
    ok(scalar(@input) > 0);

    @input = run("--testprojects=$projectName", '--platform=playmode');
    ok(scalar(@input) > 0);

    @input = run("--testprojects=$projectName", '--platform=ios');
    ok(scalar(@input) > 0);

    @input = run("--testprojects=$projectName", '--platform=Android');
    ok(scalar(@input) == 0);
}

Run_TestModeDetermination_ForProjectWithBothAssemblyDefUsingDifferentFiles:
{
    my $projectName = "ProjectWithBothAssemblyDefUsingDifferentFiles";
    my $projectPath = createEmptyUnityProject($testDataFolder, $projectName);
    make_path("$projectPath/Assets/MyTests");
    FileUtils::writeAllLines(
        "$projectPath/Assets/MyTests/AssemblyDef.asmdef", '{
        "optionalUnityReferences": [
            "TestAssemblies"
        ],
        "includePlatforms": [
            "Editor"
        ],
        "excludePlatforms": [
        ]
        }'
    );

    make_path("$projectPath/Assets/SomeOtherFolder");
    FileUtils::writeAllLines(
        "$projectPath/Assets/SomeOtherFolder/AssemblyDef.asmdef", '{
        "optionalUnityReferences": [
            "TestAssemblies"
        ],
        "includePlatforms": [
            "Android"
        ],
        "excludePlatforms": [
        ]
        }'
    );

    my $runner = createRunner();

    my @input = run("--testprojects=$projectName", '--platform=editmode');
    ok(scalar(@input) > 0);

    @input = run("--testprojects=$projectName", '--platform=Android');
    ok(scalar(@input) > 0);
}

Run_TestModeDetermination_ForProjectWithAssemblyDefWithoutTestRef:
{
    my $projectName = "ProjectWithAssemblyDefWithoutTestRef";
    my $projectPath = createEmptyUnityProject($testDataFolder, $projectName);
    make_path("$projectPath/Assets/MyTests");
    FileUtils::writeAllLines(
        "$projectPath/Assets/MyTests/AssemblyDef.asmdef", '{
        "optionalUnityReferences": [
        ],
        "includePlatforms": [
        ],
        "excludePlatforms": [
            "Android"
        ]
        }'
    );

    my $runner = createRunner();

    my @input = run("--testprojects=$projectName", '--platform=editmode');
    ok(scalar(@input) == 0);

    @input = run("--testprojects=$projectName", '--platform=playmode');
    ok(scalar(@input) == 0);

    @input = run("--testprojects=$projectName", '--platform=ios');
    ok(scalar(@input) == 0);
}

done_testing();

1;

sub createRunner
{
    my $r = new UnityTestFrameworkSuiteRunnerBase(suiteOptions => $suiteOptions);

    $r = new Test::MockObject::Extends($r);
    $r->mock(
        getName => sub
        {
            return 'RunnerName';
        }
    );

    $r->mock(
        '_getProjectDirs' => sub
        {
            return ($testDataFolder);
        }
    );

    my $options = new Options(options => $r->getOptions());
    my $runContext = RunContext::new($options);
    $r->setRunContext($runContext);
    return $r;
}

sub run
{
    my (@args) = @_;
    my $runner = createRunner();
    return TestHelpers::Runners::runAndGrabExecuteInput($runner, @args);
}

END
{
    remove_tree($testDataFolder);
}
