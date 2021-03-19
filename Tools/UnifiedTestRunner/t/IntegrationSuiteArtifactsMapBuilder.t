use warnings;
use strict;

use TestHelpers::ArrayAssertions qw (isEmpty arraysAreEqual);
use File::Temp qw (tempfile);
use Test::More;

BEGIN
{
    use_ok('ArtifactsMapBuilder');
}

FromIntegrationSuiteLog_NoTests_ProducesMapWithNoTests:
{
    my $content = '';
    my $artifactsMap = readMap($content);
    my @tests = $artifactsMap->getTests();
    isEmpty(\@tests);
}

FromIntegrationSuiteLog_OneTestNoArtifacts_ProducesResultWithOneTest:
{
    my $content = '##utp:{"type":"TestStatus", "phase":"Begin", "name":"NS.Test1"}';
    my $artifactsMap = readMap($content);
    my @tests = $artifactsMap->getTests();
    my @testsNames = map { $_->getName() } @tests;
    arraysAreEqual(\@testsNames, ['NS.Test1']);
    my @artifacts = $tests[0]->getArtifacts();
    isEmpty(\@artifacts);
}

FromIntegrationSuiteLog_OneTestOneArtifacts_ProducesResultWithOneTestAndOneArtifact:
{
    my $content = <<END_MESSAGE;
##utp:{"type":"TestStatus", "phase":"Begin", "name":"NS.Test1"}
##teamcity[publishArtifacts 'C:/Users/Unity.Automation.Log => UnityEditorStartInfo_2092/00294265-cleanlog.txt']
##teamcity[message text='Published: log1' status='NORMAL']
END_MESSAGE
    my $artifactsMap = readMap($content);
    my @tests = $artifactsMap->getTests();
    my @testsNames = map { $_->getName() } @tests;
    arraysAreEqual(\@testsNames, ['NS.Test1']);
    my @artifacts = $tests[0]->getArtifacts();
    arraysAreEqual(\@artifacts, ['log1']);
}

GetArtifactsForTest_NotArtifacts_ReturnsEmptyArray:
{
    my $content = <<END_MESSAGE;
##utp:{"type":"TestStatus", "phase":"Begin", "name":"NS.Test1"}
##teamcity[publishArtifacts 'C:/Users/Unity.Automation.Log => UnityEditorStartInfo_2092/00294265-cleanlog.txt']
END_MESSAGE
    my $artifactsMap = readMap($content);
    my @artifacts = $artifactsMap->getArtifactsForTest('NS.Test1');
    isEmpty(\@artifacts);
}

GetArtifactsForTest_ReturnsArtifactsForGivenTests:
{
    my $content = <<END_MESSAGE;
##utp:{"type":"TestStatus", "phase":"Begin", "name":"NS.Test1"}
##teamcity[publishArtifacts 'C:/Users/Unity.Automation.Log => UnityEditorStartInfo_2092/00294265-cleanlog.txt']
##teamcity[message text='Published: log1' status='NORMAL']
##teamcity[message text='Published: log2' status='NORMAL']
END_MESSAGE
    my $artifactsMap = readMap($content);
    my @artifacts = $artifactsMap->getArtifactsForTest('NS.Test1');
    arraysAreEqual(\@artifacts, ['log1', 'log2']);
}

GetArtifactsForTest_ArtifactsRootSpecified_ReturnsArtifactsForGivenTestsWithArtifactsRootCut:
{
    my $content = <<END_MESSAGE;
##utp:{"type":"TestStatus", "phase":"Begin", "name":"NS.Test1"}
##teamcity[publishArtifacts 'C:/Users/Unity.Automation.Log => UnityEditorStartInfo_2092/00294265-cleanlog.txt']
##teamcity[message text='Published: /a/b/log1' status='NORMAL']
##teamcity[message text='Published: /a/b/log2' status='NORMAL']
END_MESSAGE
    my $artifactsMap = readMap($content, '/a/b/');
    my @artifacts = $artifactsMap->getArtifactsForTest('NS.Test1');
    arraysAreEqual(\@artifacts, ['log1', 'log2']);
}

GetArtifactsForTest_NoArtifacts_ReturnsEmptyArray:
{
    my $content = <<END_MESSAGE;
##utp:{"type":"TestStatus", "phase":"Begin", "name":"NS.Test1"}
END_MESSAGE
    my $artifactsMap = readMap($content, '/a/b/');
    my @artifacts = $artifactsMap->getArtifactsForTest('NS.Test1');
    isEmpty(\@artifacts);
}

GetArtifactsForTest_ArtifactsNameSpecifiedAfterSummetn_ReturnsEmptyArray:
{
    my $content = <<END_MESSAGE;
##utp:{"type":"TestStatus", "phase":"Begin", "name":"NS.Test1"}
Test Run Summary: 3, Errors: 0, Failures: 3, Inconclusive: 0, Time: 0.040981 seconds
Errors and Failures:
1) Test Failure : Unity.CommonTools.Test.KatanaArtifactReporterTests.CanPublishFileWithDifferentName
     Expected: String ending with "renamed.txt
"
  But was:  "##teamcity[publishArtifacts 'C:/Users/yan/AppData/Local/Temp/somepath/somefile.txt => renamed.txt']
##teamcity[message text='Published: C:\\artifact\\renamed.txt' status='NORMAL']
"
at NUnit.Framework.StringAssert.EndsWith (System.String expected, System.String actual, System.String message, System.Object[] args) [0x00000] in <filename unknown>:0
at NUnit.Framework.StringAssert.EndsWith (System.String expected, System.String actual) [0x00000] in <filename unknown>:0
END_MESSAGE
    my $artifactsMap = readMap($content);
    my @artifacts = $artifactsMap->getArtifactsForTest('NS.Test1');
    isEmpty(\@artifacts);
}

GetArtifacts_ReturnsAllUnassignedArtifacts:
{
    my $content = <<END_MESSAGE;
##teamcity[message text='Published: log1' status='NORMAL']
##teamcity[message text='Published: log2' status='NORMAL']
END_MESSAGE
    my $artifactsMap = readMap($content);
    my @artifacts = $artifactsMap->getArtifacts();
    arraysAreEqual(\@artifacts, ['log1', 'log2']);
}

GetArtifacts_AssignsThemToTheLastExecutedTest:
{
    my $content = <<END_MESSAGE;
##utp:{"type":"TestStatus", "phase":"Begin", "name":"NS.Test1"}
##utp:{"type":"TestStatus", "phase":"End", "name":"NS.Test1"}
##teamcity[message text='Published: Cleanlog.txt' status='NORMAL']
##teamcity[message text='Published: Rawlog.txt' status='NORMAL']
END_MESSAGE
    my $artifactsMap = readMap($content);
    my @artifacts = $artifactsMap->getArtifactsForTest('NS.Test1');
    arraysAreEqual(\@artifacts, ['Cleanlog.txt', 'Rawlog.txt']);
}

done_testing();

sub readMap
{
    my ($content, $artifactsRoot) = @_;
    my ($fh, $tempFileName) = tempfile();
    print($fh $content);
    close($fh);
    my $artifactsMap = ArtifactsMapBuilder::fromIntegrationSuiteLog($tempFileName, $artifactsRoot);
    return $artifactsMap;
}
