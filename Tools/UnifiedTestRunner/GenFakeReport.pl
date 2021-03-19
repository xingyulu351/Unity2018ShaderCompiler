use warnings;
use strict;

use File::Spec;
use File::Basename qw (dirname);
use lib File::Spec->rel2abs(dirname($0));

use SuiteResult;
use TestResult;
use Report::TestSessionReportBuilder;
use Getopt::Long;
use File::Temp qw (tempdir);
use File::Path qw (mkpath);
use Dirs;
use lib Dirs::external_root();
use File::Spec::Functions qw (catfile catdir);
use Switch;
use Hoarder::Collectors;
use HtmlReportGenerator;
use Getopt::Long;
use File::Basename;
use File::Copy;
use PerformanceDataPostProcessor;
use JSON;

my $artifactsPath;
my $partitionIndex;

sub main
{
    GetOptions(
        "artifacts_path=s" => \$artifactsPath,
        "partition-index=s" => \$partitionIndex,
    );
    if (not defined($artifactsPath))
    {
        $artifactsPath = tempdir();
    }
    print("Generating report $artifactsPath\n");
    if (-e "$artifactsPath/TestReportData.json")
    {
        unlink "$artifactsPath/TestReportData.json";
    }

    my @suiteResults = getSuiteResults();
    my @collectors = Hoarder::Collectors::getAllCollectors();
    my @commandLine =
        ('test.pl', '-suite=native', '--suite=graphics', '--debugallocator', '--testfilter=FILTER', '--scriptingbackend=il2cpp', '--device=nexus4');

    HtmlReportGenerator::generate(
        suiteResults => \@suiteResults,
        collectors => \@collectors,
        artifactsRoot => $artifactsPath,
        commandLine => \@commandLine
    );
}

main();

sub getSuiteResults
{
    my $native = getNativeSuiteResults();
    my $graphics = getGraphicsSuiteResults();
    my $performanceRuntime = getPerformanceRuntimeSuiteResults();
    my $runtime = getRuntimeTestsWithMemoryLeaksLimitExceeded();

    my @suiteResults = ($native, $graphics, $performanceRuntime, $runtime);
    return @suiteResults;
}

sub getNativeSuiteResults
{
    my $suiteResult = new SuiteResult(
        name => 'native',
        platform => 'standalone',
        artifacts => [createArtifact('native'), createArtifact('native')],
        partitionIndex => $partitionIndex,
    );
    $suiteResult->forceFailed('suite failure reason1');
    $suiteResult->forceFailed("suite failure\n reason2");

    my @suiteErrors = ["suite error1", "suite error 2"];
    $suiteResult->{errors} = \@suiteErrors;

    $suiteResult->setDescription($suiteResult->getName() . "Description");
    $suiteResult->setDetails($suiteResult->getName() . " details");
    my @testResults = generateTestForAllTypes('native');
    foreach my $tr (@testResults)
    {
        $suiteResult->addTestResult($tr);
    }
    return $suiteResult;
}

sub getGraphicsSuiteResults
{
    my $suite = 'graphics';
    my $utrRoot = Dirs::UTR_root();
    my $graphicsTestDataRoot = "$utrRoot/t/TestData/GraphicsTests";

    my $trSuccess = generateTest(
        suite => $suite,
        state => TestResult::SUCCESS,
    );

    my $trFailure = generateTest(
        suite => $suite,
        state => TestResult::FAILURE,
        artifacts => [
            createArtifactFromExistingFile($suite, catfile($graphicsTestDataRoot, "correct.png")),
            createArtifactFromExistingFile($suite, catfile($graphicsTestDataRoot, "rendered.png")),
            createArtifactFromExistingFile($suite, catfile($graphicsTestDataRoot, "diff.png"))
        ],
        message => "Images don't match",
        errors => ['test error1', "test error2\ntwo lines and \ttab"]
    );

    my $trInconclusive = generateTest(
        suite => $suite,
        state => TestResult::INCONCLUSIVE,
        messsage => 'Player crashed?'
    );

    my $suiteResult = new SuiteResult(
        name => $suite,
        platform => 'standalone',
        artifacts => [createArtifact($suite), createArtifact($suite)],
        partitionIndex => $partitionIndex,
    );
    $suiteResult->setDetails($suiteResult->getName() . " details");
    $suiteResult->addTestResult($trSuccess);
    $suiteResult->addTestResult($trFailure);
    $suiteResult->addTestResult($trInconclusive);

    return $suiteResult;
}

sub getPerformanceRuntimeSuiteResults
{
    my $suite = 'performance-runtime';

    my $trSuccess1 = generateTest(
        suite => $suite,
        state => TestResult::SUCCESS,
    );

    my $trSuccess2 = generateTest(
        suite => $suite,
        state => TestResult::SUCCESS,
    );

    my $testName1 = $trSuccess1->getTest();
    my $testName2 = $trSuccess2->getTest();
    my $json = <<END_JSON;
[
  {
    "testName": "$testName1",
    "state": 4,
    "sampleGroupResults": [
      {
        "sampleGroupName": "FrameTime",
        "baselineValue": 0,
        "threshold": 0.1,
        "increaseIsBetter": false,
        "aggregationType": "Median",
        "percentile": 0,
        "currentValue": 5.934399999999869
      },
      {
        "sampleGroupName": "TotalTime",
        "baselineValue": 0,
        "threshold": 0.1,
        "increaseIsBetter": false,
        "aggregationType": "Median",
        "percentile": 0,
        "currentValue": 1360.3044
      }
    ]
  },
  {
    "testName": "$testName2",
    "state": 4,
    "sampleGroupResults": [
      {
        "sampleGroupName": "FrameTime",
        "baselineValue": 0,
        "threshold": 0.1,
        "increaseIsBetter": false,
        "aggregationType": "Median",
        "percentile": 0,
        "currentValue": 5.934399999999869
      },
      {
        "sampleGroupName": "TotalTime",
        "baselineValue": 0,
        "threshold": 0.1,
        "increaseIsBetter": false,
        "aggregationType": "Median",
        "percentile": 0,
        "currentValue": 1360.3044
      }
    ]
  }
]
END_JSON

    my $suiteResult = new SuiteResult(
        name => $suite,
        platform => 'standalone',
        artifacts => [createArtifact($suite), createArtifact($suite)],
        partitionIndex => $partitionIndex,
    );
    $suiteResult->setDetails($suiteResult->getName() . " details");
    $suiteResult->addTestResult($trSuccess1);
    $suiteResult->addTestResult($trSuccess2);
    PerformanceDataPostProcessor::embedPerformanceData(jsonToObj($json), $suiteResult);
    return $suiteResult;
}

sub getRuntimeTestsWithMemoryLeaksLimitExceeded
{
    my $suite = 'runtime';
    my $trSuccess = generateTest(
        suite => $suite,
        state => TestResult::SUCCESS,
    );

    my $suiteResult = new SuiteResult(
        name => $suite,
        platform => 'editor',
        partitionIndex => $partitionIndex,
    );

    my $json = <<END_JSON;
{
    "type": "MemoryLeaks",
    "allocatedMemory": 102445,
    "memoryLabels": [
        {
            "NewDelete": 1435
        },
        {
            "DynamicArray": 3235
        }
    ]
}
END_JSON
    $suiteResult->setDetails($suiteResult->getName() . " details");
    $suiteResult->addData('MemoryLeaks', jsonToObj($json));
    $suiteResult->addTestResult($trSuccess);
    $suiteResult->forceFailed('Memory leaks limit exceeded');
    $suiteResult->forceFailed('Something elese happened');
    return $suiteResult;
}

my $artifactsCounter = 0;

sub createArtifactFromExistingFile
{
    my ($suite, $file) = @_;
    my $suiteRoot = catdir($artifactsPath, $suite);
    if (not -d $suiteRoot)
    {
        mkpath($suiteRoot);
    }

    $artifactsCounter++;
    my $destFile = catfile($suiteRoot, $artifactsCounter . fileparse($file));
    copy($file, $destFile);
    return FileUtils::removeSubPath($destFile, $artifactsPath);
}

sub createArtifact
{
    my ($suite) = @_;
    my $suiteRoot = catdir($artifactsPath, $suite);
    if (not -d $suiteRoot)
    {
        mkpath($suiteRoot);
    }

    $artifactsCounter++;
    my $fileName = catfile($suiteRoot, "file$artifactsCounter.txt");
    FileUtils::writeAllLines($fileName, ("fake $fileName line 1", "fake $fileName line 2"));
    return FileUtils::removeSubPath($fileName, $artifactsPath);
}

my $testsCounter = 0;

sub generateTestForAllTypes
{
    my ($suite) = @_;
    my $trInconclusive = generateTest(
        suite => $suite,
        state => TestResult::INCONCLUSIVE,
        artifacts => [createArtifact($suite), createArtifact($suite)],
        messsage => 'Player crashed?'
    );

    my $trSkipped = generateTest(
        suite => $suite,
        state => TestResult::SKIPPED,
        artifacts => [createArtifact($suite), createArtifact($suite)],
        messsage => 'Skipped test'
    );

    my $trIgnored = generateTest(
        suite => $suite,
        state => TestResult::IGNORED,
        artifacts => [createArtifact($suite), createArtifact($suite)],
        messsage => 'Ignored for no reason'
    );

    my $trSuccess = generateTest(
        suite => $suite,
        state => TestResult::SUCCESS,
        artifacts => [createArtifact($suite), createArtifact($suite)]
    );

    my $trFailure = generateTest(
        suite => $suite,
        state => TestResult::FAILURE,
        artifacts => [createArtifact($suite), createArtifact($suite)],
        message => "Failure line 1\n Failure line 2\nMessage\twith\ttabs",
        stackTrace => "stack trace line1\nstack\ntrace\nline2"
    );

    my $trError = generateTest(
        suite => $suite,
        state => TestResult::ERROR,
        artifacts => [createArtifact($suite), createArtifact($suite)],
        message => "Failure line 1\n Failure line 2\n Message\twith\tabs",
        stackTrace => "stack trace line1\nstack trace line2"
    );

    return ($trInconclusive, $trSkipped, $trIgnored, $trSuccess, $trFailure, $trError);
}

sub generateTest
{
    my (%args) = @_;
    my $testName = generateTestName(%args);
    my $tr = new TestResult();
    $tr->setTest($testName);
    $tr->setFixture('fixture');
    $tr->setTime($testsCounter * 1000);
    $tr->setState($args{state});

    if (defined($args{message}))
    {
        $tr->setMessage($args{message});
    }

    if (defined($args{stackTrace}))
    {
        $tr->setStackTrace($args{stackTrace});
    }

    if (defined($args{artifacts}))
    {
        $tr->setArtifacts(@{ $args{artifacts} });
    }

    if ($args{errors})
    {
        $tr->{errors} = $args{errors};
    }

    return $tr;
}

sub generateTestName
{
    my (%args) = @_;
    $testsCounter++;
    my $testName = "Unity.Fake.$args{suite}";
    $testName .= ".$testsCounter." . TestResult::stateToString($args{state});
    return $testName;
}
