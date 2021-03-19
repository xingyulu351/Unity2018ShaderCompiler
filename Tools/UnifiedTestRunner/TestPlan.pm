package TestPlan;

use warnings;
use strict;

use TestPlanItem;
use FileUtils;
use Dirs;
use lib Dirs::external_root();
use JSON;
use UnityTestProtocol::Utils;

sub new
{
    my $package = shift;
    my %params = @_;
    my $obj = {
        testItems => [],
        totalTestCases => 0,
        totalIgnored => 0
    };
    bless($obj, $package);
    return $obj;
}

sub registerTest
{
    my ($this, $testItem) = @_;
    $this->{ $testItem->getName() } = $testItem;
    my $numTestCases = $testItem->getTestCaseCount();
    if ($numTestCases eq 0)
    {
        $this->{totalTestCases} += 1;    #count ignored test as one, to build correct test plan
        $this->{totalIgnored} += 1;
    }
    else
    {
        $this->{totalTestCases} += $testItem->getTestCaseCount();
    }
    push(@{ $this->{testItems} }, $testItem);
}

sub getTestItem
{
    my ($this, $name) = @_;
    return $this->{$name};
}

sub getAllTestItems
{
    my ($this) = @_;
    return @{ $this->{testItems} };
}

sub getTotalTests
{
    my ($this) = @_;
    return scalar(@{ $this->{testItems} });
}

sub getTotalTestCases
{
    my ($this) = @_;
    return $this->{totalTestCases};
}

sub getTotalIgnored
{
    my ($this) = @_;
    return $this->{totalIgnored};
}

sub getTestNames
{
    my ($this) = @_;
    my @result = ();
    foreach ($this->getAllTestItems())
    {
        push(@result, $_->getName());
    }
    return @result;
}

sub getTestPlanFromTestProtocolMessage
{
    my ($msg) = @_;
    my $testPlan = new TestPlan();
    if (not defined($msg))
    {
        return $testPlan;
    }
    foreach my $test (@{ $msg->{tests} })
    {
        my $testPlanItem = TestPlanItem->new(name => $test, testCaseCount => 1);
        $testPlan->registerTest($testPlanItem);
    }
    return $testPlan;
}

sub loadTestPlanFromTestListerOutput
{
    my ($filename) = @_;
    my @testItems = ();
    my @lines = ();
    my $openRes = open(my $fh, '<', $filename);
    my $testPlan = TestPlan->new();
    if (not defined($openRes))
    {
        return $testPlan;
    }

    while (<$fh>)
    {
        my $line = $_;
        chomp($line);
        my ($testname, $testCaseCount) = ($line =~ m /\[(.*)\]\s+\[(\d+)\]/);
        if ($testname and defined($testCaseCount))
        {
            my $testPlanItem = TestPlanItem->new(name => $testname, testCaseCount => $testCaseCount);
            $testPlan->registerTest($testPlanItem);
        }
    }

    close($fh);
    return $testPlan;
}

sub loadFromPlainTestListFile
{
    my ($testListFileName) = @_;

    my @tests = FileUtils::readAllLines($testListFileName);
    my $testPlan = new TestPlan();
    foreach my $t (@tests)
    {
        my $testPlanItem = TestPlanItem->new(name => $t, testCaseCount => 1);
        $testPlan->registerTest($testPlanItem);
    }
    return $testPlan;
}

sub loadFromPartitionsJsonFile
{
    my ($fileName, $partitionIndex, $testfilter) = @_;
    my $text = join(q (), FileUtils::readAllLines($fileName));
    return loadFromPartitionsJson($text, $partitionIndex, $testfilter);
}

sub loadFromPartitionsJson
{
    my ($jsonText, $partitionIndex, $testfilter) = @_;
    my $data = jsonToObj($jsonText);
    my @partitions = @{ $data->{Partitions} };
    my $partition = $partitions[$partitionIndex];
    my $testPlan = new TestPlan();
    foreach my $t (@{$partition})
    {
        if ($testfilter and not($t->{Name} =~ /$testfilter/))
        {
            next;
        }

        my $testPlanItem = TestPlanItem->new(name => $t->{Name}, testCaseCount => 1);
        $testPlan->registerTest($testPlanItem);
    }
    return $testPlan;
}

sub readTestPlanFromMachineLog
{
    my ($fileName) = @_;
    my $msg = UnityTestProtocol::Utils::getFirst(fileName => $fileName, predicate => sub { return $_[0]->{type} eq 'TestPlan'; });
    if (not defined($msg))
    {
        croak("Can not read the test plan from $fileName");
    }
    my $testPlan = TestPlan::getTestPlanFromTestProtocolMessage($msg);
    return $testPlan;
}

1;
