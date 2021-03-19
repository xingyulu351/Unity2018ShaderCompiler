use warnings;
use strict;

use Test::More;
use TestResult;
use SuiteResult;

use Dirs;
use lib Dirs::external_root();
use JSON;

BEGIN { use_ok('PerformanceDataPostProcessor'); }

can_ok('PerformanceDataPostProcessor', 'embedPerformanceData');

embedPerformanceData_OneTest_EmbedsTestDataIntoIt:
{
    my $json = <<END_JSON;
[
  {
    "testName": "SceneBased_EmptyScene_Empty",
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
    my $suiteResult = new SuiteResult();
    my $tr = new TestResult();
    $tr->setTest("SceneBased_EmptyScene_Empty");
    my @testResults = ($tr);
    $suiteResult->addTestResults(@testResults);

    PerformanceDataPostProcessor::embedPerformanceData(jsonToObj($json), $suiteResult);
    my $data = $tr->getData();
    ok(defined($data));
    my $testPerformanceData = $data->{performanceTestResults};
    is($testPerformanceData->{testName}, 'SceneBased_EmptyScene_Empty');
}

#TODO: warning is printed when no matching test is found
done_testing();
