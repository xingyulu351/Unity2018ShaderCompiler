package NUnitXmlParser;

use warnings;
use strict;
use Dirs;
use lib Dirs::external_root();
use XML::XPath;
use Data::Dumper;
use TestResult;
use Switch;
use Readonly;
use Carp qw( croak );
use StringUtils;

Readonly my $MILLISECONDS_IN_SECOND => 1000;

sub parse
{
    my ($xml) = @_;
    my %result;
    $result{testResults} = [];

    my $xp = XML::XPath->new($xml);

    my @nodes = $xp->findnodes('//test-case');
    my @testResults = _createTestResults(@nodes);
    $result{testResults} = \@testResults;

    return %result;
}

sub _createTestResults
{
    my (@testCaseNodes) = @_;
    my @result;
    foreach my $tc (@testCaseNodes)
    {
        push(@result, _createTestResult($tc));
    }
    return @result;
}

sub _createTestResult
{
    my ($testCaseNode) = @_;
    my $fullname = $testCaseNode->getAttribute('fullname');
    if ($fullname)
    {
        return _createTestResult35($testCaseNode);
    }
    return _createTestResult26($testCaseNode);
}

sub _createTestResult26
{
    my ($testCaseNode) = @_;
    my $testResult = new TestResult();
    $testResult->setTest($testCaseNode->getAttribute('name'));
    $testResult->setState(TestResult::SUCCESS);

    my $time = $testCaseNode->getAttribute('time');

    $testResult->setTime($time * $MILLISECONDS_IN_SECOND);

    my $state = _stringToState26($testCaseNode->getAttribute('result'));
    $testResult->setState($state);

    my $message = _getMessage($testCaseNode);
    $testResult->setMessage($message);

    my $stackTrace = _getStackTrace($testCaseNode);
    $testResult->setStackTrace($stackTrace);

    my $reason = _getReason($testCaseNode);
    $testResult->setReason($reason);

    return $testResult;
}

sub _createTestResult35
{
    my ($testCaseNode) = @_;
    my $testResult = new TestResult();
    $testResult->setTest($testCaseNode->getAttribute('fullname'));
    my $state = _stringToState35($testCaseNode);
    $testResult->setState($state);

    my $time = $testCaseNode->getAttribute('duration');
    $testResult->setTime(int($time * $MILLISECONDS_IN_SECOND));

    my $message = _getMessage($testCaseNode);
    $testResult->setMessage($message);

    my $stackTrace = _getStackTrace($testCaseNode);
    $testResult->setStackTrace($stackTrace);

    my $reason = _getReason($testCaseNode);
    $testResult->setReason($reason);

    return $testResult;
}

sub _getMessage
{
    my ($testCaseNode) = @_;
    return _getNodeData($testCaseNode, 'failure/message');
}

sub _getStackTrace
{
    my ($testCaseNode) = @_;
    return _getNodeData($testCaseNode, 'failure/stack-trace');
}

sub _getReason
{
    my ($testCaseNode) = @_;
    return _getNodeData($testCaseNode, 'reason/message');
}

sub _getNodeData
{
    my ($testCaseNode, $childNodeName) = @_;
    my ($childNode) = $testCaseNode->findnodes($childNodeName);
    if (not defined($childNode))
    {
        return;
    }
    my @childNodes = $childNode->getChildNodes();
    if (scalar(@childNodes) == 0)
    {
        return;
    }
    my $firstChild = $childNodes[0];
    if (not defined($firstChild))
    {
        return;
    }
    return $firstChild->getData();
}

sub _stringToState26
{
    my ($stateStr) = @_;
    switch (uc($stateStr))
    {
        case 'INCONCLUSIVE' { return TestResult::INCONCLUSIVE; }
        case 'NOTRUNNABLE' { return TestResult::NOTRUNNABLE; }
        case 'SKIPPED' { return TestResult::SKIPPED; }
        case 'IGNORED' { return TestResult::IGNORED; }
        case 'SUCCESS' { return TestResult::SUCCESS; }
        case 'FAILURE' { return TestResult::FAILURE; }
        case 'ERROR' { return TestResult::ERROR; }
        else { croak("Unsupported test result value $stateStr") }
    }
}

sub _stringToState35
{
    my ($testCaseNode) = @_;
    my $stateStr = $testCaseNode->getAttribute('result');
    switch (uc($stateStr))
    {
        case 'PASSED' { return TestResult::SUCCESS; }
        case 'FAILED'
        {
            my $label = $testCaseNode->getAttribute('label');
            if (StringUtils::isNullOrEmpty($label))
            {
                return TestResult::FAILURE;
            }
            if (uc($label) eq 'ERROR')
            {
                return TestResult::ERROR;
            }

            if (uc($label) eq 'INVALID')
            {
                return TestResult::NOTRUNNABLE;
            }
            croak("Unsupported label value '$label'")
        }
        case 'INCONCLUSIVE' { return TestResult::INCONCLUSIVE; }
        case 'SKIPPED'
        {
            my $runState = $testCaseNode->getAttribute('runstate');
            if (uc($runState) eq 'SKIPPED')
            {
                return TestResult::SKIPPED;
            }
            return TestResult::IGNORED;
        }
        else { croak("Unsupported test result value '$stateStr'") }
    }
}

sub _getNUnitVersion
{
    my ($xp) = @_;
    my @nodes = $xp->findnodes('//environment');
    if (scalar(@nodes) == 0)
    {
        my @nodes = $xp->findnodes('test-suite');
        if (scalar(@nodes) > 0)
        {
            my $id = $nodes[0]->getAttribute('id');
            if (defined($id))
            {
                return "3.5.0.0";
            }
        }
    }
    my $frameworkVersion = $nodes[0]->getAttribute('framework-version');
    if (StringUtils::isNullOrEmpty($frameworkVersion))
    {
        $frameworkVersion = $nodes[0]->getAttribute('nunit-version');
    }
    return $frameworkVersion;
}

sub isNUnit26
{
    my ($version) = @_;
    if ($version =~ m/2[.]6[.].*/ or $version eq 'AllIn1Runner' or $version eq 'AssetImport')
    {
        return 1;
    }
    return 0;
}

sub isNUnit35
{
    my ($version) = @_;
    if ($version =~ m/3[.]5[.].*/)
    {
        return 1;
    }
    return 0;
}

1;
