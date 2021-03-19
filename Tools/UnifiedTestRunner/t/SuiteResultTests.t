use Test::More;

use warnings;
use strict;

use TestResult;

use TestHelpers::ArrayAssertions qw (arraysAreEqual);
use FileUtils qw (readAllLines);
use Dirs;
use Test::MockModule;
use lib Dirs::external_root();
use JSON;
use Report::TestSessionReportBuilder;
use Storable qw (dclone);
use Test::More;
use File::Spec::Functions qw (catfile);

BEGIN { use_ok('SuiteResult'); }

#TODO: croak if runner does not support parallel test execution

Creation:
{
    can_ok('SuiteResult', 'new');
    my $suiteResult = new SuiteResult(name => 'native', platform => 'standalone');
    isa_ok($suiteResult, 'SuiteResult');
    can_ok($suiteResult, 'getName');
    can_ok($suiteResult, 'getPlatform');

    is($suiteResult->getName(), 'native');
    is($suiteResult->getPlatform(), 'standalone');
}

GetSuite_ReturnsExpectedSuite:
{
    my $suiteResult = new SuiteResult(name => 'native', platform => 'standalone');
    can_ok($suiteResult, 'addTestResult');

    my $tr = TestResult->new();
    $suiteResult->addTestResult($tr);

    can_ok($suiteResult, 'getAllResults');
    my @results = $suiteResult->getAllResults();
    is(scalar(@results), 1);
    is($results[0]->getSuite(), 'native');
}

GetAllTestResults_ReturnsThem:
{
    my $suiteResult = new SuiteResult(name => 'native', platform => 'standalone');
    can_ok($suiteResult, 'addTestResults');
    my @testResults = (TestResult->new(), TestResult->new());
    $suiteResult->addTestResults(@testResults);
    my @results = $suiteResult->getAllResults();
    is(scalar(@results), 2);
}

GetSetTestResultXml_StoresTestResultXmlFilePathInSuiteResult:
{
    my $suiteResult = new SuiteResult(name => 'A', platform => 'B');
    can_ok($suiteResult, 'setTestResultXmlFilePath');
    $suiteResult->setTestResultXmlFilePath('C');
    can_ok($suiteResult, 'getTestResultXmlFilePath');
    is($suiteResult->getTestResultXmlFilePath('C'), 'C');
}

GetArtifacts_ReturnsArtifacts:
{
    my $suiteResult = new SuiteResult(name => 'A', platform => 'B', artifacts => ['a1', 'a2']);
    my @artifacts = $suiteResult->getArtifacts();
    arraysAreEqual(\@artifacts, ['a1', 'a2']);
}

GetOverridedOptions_ReturnsAddedOptions:
{
    my $suiteResult = new SuiteResult(name => 'A');
    $suiteResult->addOptionOverride(name => 'commandlineOptionName', value => 'value');
    my @options = $suiteResult->getOverridedOptions();
    is(scalar(@options), 1);
    is($options[0]->{name}, 'commandlineOptionName');
    is($options[0]->{value}, 'value');
}

ForceFailure_FailsSuiteResultAndSetsReason:
{
    my $suiteResult = new SuiteResult(
        name => 'native',
        platform => 'standalone'
    );

    is($suiteResult->isForcibilyFailed(), 0);
    $suiteResult->forceFailed('memory leaks limit exceeded');
    is($suiteResult->isForcibilyFailed(), 1);
    my @reasons = $suiteResult->getFailureReasons();
    arraysAreEqual(\@reasons, ['memory leaks limit exceeded']);
}

GetDescription_ReturnsIt_ReturnsSuiteName:
{
    my $suiteResult = new SuiteResult(
        name => 'editor',
        platform => 'standalone'
    );
    $suiteResult->setDescription('Undo project');
    is($suiteResult->getDescription(), 'Undo project');
}

PartitionId:
{
    my $suiteResult = new SuiteResult();
    $suiteResult->setPartitionIndex('12');
    is($suiteResult->getPartitionIndex(), '12');
}

CanLoadSuiteResultFromJson:
{
    my $testResult1 = new TestResult();
    $testResult1->setState(TestResult::SUCCESS);
    $testResult1->setTime(123);
    $testResult1->setTest('Test1');
    $testResult1->setArtifacts('t1', 't2');

    my $srcSuiteResult = new SuiteResult(
        name => 'fake_suite',
        platform => 'fake_platform',
        artifacts => ['a1', 'a2'],
        description => 'description text',
        artifacts => ['a1', 'a2'],
        minimalCommandLine => ['--testproject=abc'],
        partitionId => 2,
    );

    $srcSuiteResult->setTestResultXmlFilePath('/path_to_tr.xml');
    $srcSuiteResult->addTestResult($testResult1);

    my $restoredSuiteResult = SuiteResult::fromJson(generate($srcSuiteResult));
    is_deeply($restoredSuiteResult, $srcSuiteResult);
}

CanReadSuiteResultFromTestReportDataJson:
{
    my $jsonReport = catfile(Dirs::UTR_root(), 't/TestData/TestReportJson/TestReportData.json');
    my $testReport = FileUtils::readJsonObj($jsonReport);
    my $suite = $testReport->{suites}->[0];
    my $sr = SuiteResult::fromJson($suite);
    is($sr->{name}, 'graphics');
    is(scalar(@{ $sr->{results} }), 5);
}

done_testing();

sub generate
{
    my ($suiteResult) = @_;
    my %data = Report::TestSessionReportBuilder::buildData(suiteResults => [$suiteResult]);
    my $json = JSON->new(utf8 => 1, convblessed => 1);
    my $jsonText = $json->to_json(\%data);
    my $restoredData = jsonToObj($jsonText);
    my @suites = @{ $restoredData->{suites} };
    return $suites[0];
}
