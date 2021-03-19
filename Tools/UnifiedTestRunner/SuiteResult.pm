package SuiteResult;

use warnings;
use strict;

use StringUtils;
use Dirs;
use lib Dirs::external_root();
use JSON;
use TestResult;

sub new
{
    my $pkg = shift();
    my %params = @_;
    my $instance = {
        name => $params{name},
        platform => $params{platform},
        testresultsxml => $params{testresultsxml},
        results => [],
        artifactsmap => $params{artifactsmap},
        artifacts => [],
        overridedOptions => [],
        data => {},
        forciblyFailed => 0,
        failureReasons => [],
        zeroTestsAreOk => 0,
        partitionIndex => $params{partitionIndex},
        description => $params{description},
        minimalCommandLine => $params{minimalCommandLine},
    };

    if ($params{artifacts})
    {
        $instance->{artifacts} = $params{artifacts};
    }

    return bless($instance, $pkg);
}

sub setName
{
    my ($this, $value) = @_;
    $this->{name} = $value;
}

sub getName
{
    my ($this) = @_;
    return $this->{name};
}

sub getPlatform
{
    my ($this) = @_;
    return $this->{platform};
}

sub addTestResult
{
    my ($this, $result) = @_;
    $result->setParent($this);
    if (not $result->getArtifacts() and $this->{artifactsmap})
    {
        my @artifacts = $this->{artifactsmap}->getArtifactsForTest($result->getTest());
        $result->setArtifacts(@artifacts);
    }
    push(@{ $this->{results} }, $result);
}

sub addArtifact
{
    my ($this, $artifact) = @_;
    push(@{ $this->{artifacts} }, $artifact);
}

sub addTestResults
{
    my ($this, @results) = @_;
    foreach my $tr (@results)
    {
        $this->addTestResult($tr);
    }
}

sub getAllResults
{
    my ($this, $result) = @_;
    return @{ $this->{results} };
}

sub setTestResultXmlFilePath
{
    my ($this, $value) = @_;
    $this->{testresultsxml} = $value;
}

sub getTestResultXmlFilePath
{
    my ($this) = @_;
    return $this->{testresultsxml};
}

sub getArtifactsMap
{
    my ($this) = @_;
    return $this->{artifactsmap};
}

sub getArtifacts
{
    my ($this) = @_;
    my @result = @{ $this->{artifacts} };
    return @result;
}

sub addOptionOverride
{
    my ($this, %override) = @_;
    push(@{ $this->{overridedOptions} }, \%override);
}

sub getOverridedOptions
{
    my ($this, %override) = @_;
    return @{ $this->{overridedOptions} };
}

sub addData
{
    my ($this, $keyName, $value) = @_;
    $this->{data}->{$keyName} = $value;
}

sub getData
{
    my ($this) = @_;
    return $this->{data};
}

sub forceFailed
{
    my ($this, $reason) = @_;
    if (not StringUtils::isNullOrEmpty($reason))
    {
        push(@{ $this->{failureReasons} }, $reason);
    }
    $this->{forciblyFailed} = 1;
}

sub isForcibilyFailed
{
    my ($this) = @_;
    return $this->{forciblyFailed};
}

sub getFailureReasons
{
    my ($this) = @_;
    return @{ $this->{failureReasons} };
}

sub setDetails
{
    my ($this, $value) = @_;
    $this->{details} = $value;
}

sub getDetails
{
    my ($this) = @_;
    return $this->{details};
}

sub getMinimalCommandLine
{
    my ($this, @commandLine) = @_;
    $this->{minimalCommandLine} = \@commandLine;
}

sub setMinimalCommandLine
{
    my ($this) = @_;
    return @{ $this->{minimalCommandLine} };
}

sub setDescription
{
    my ($this, $value) = @_;
    $this->{description} = $value;
}

sub getDescription
{
    my ($this) = @_;
    return $this->{description};
}

sub getZeroTestsAreOk
{
    my ($this) = @_;
    return $this->{zeroTestsAreOk};
}

sub setZeroTestsAreOk
{
    my ($this, $value) = @_;
    $this->{zeroTestsAreOk} = $value;
}

sub setPartitionIndex
{
    my ($this, $value) = @_;
    $this->{partitionIndex} = $value;
}

sub getPartitionIndex
{
    my ($this) = @_;
    return $this->{partitionIndex};
}

sub fromJson
{
    my ($jsonEntry) = @_;
    my $result = new SuiteResult(
        name => $jsonEntry->{name},
        platform => $jsonEntry->{platform},
    );
    $result->{testresultsxml} = $jsonEntry->{testresultsxml};
    $result->{partitionIndex} = $jsonEntry->{partitionIndex};
    $result->{description} = $jsonEntry->{description};
    $result->{minimalCommandLine} = $jsonEntry->{minimalCommandLine};

    foreach my $art (@{ $jsonEntry->{artifacts} })
    {
        $result->addArtifact($art);
    }
    foreach my $test (@{ $jsonEntry->{tests} })
    {
        $result->addTestResult(_makeTestResult($test));
    }
    return $result;
}

sub _makeTestResult
{
    my ($jsonTest) = @_;
    my $tr = new TestResult();
    $tr->setTest($jsonTest->{name});
    $tr->setMessage($jsonTest->{message});
    $tr->setFixture($jsonTest->{fixture});
    $tr->setStackTrace($jsonTest->{stackTrace});
    $tr->setTime($jsonTest->{time});
    $tr->setReason($jsonTest->{reason});
    $tr->setState($jsonTest->{state});
    $tr->setArtifacts(@{ $jsonTest->{artifacts} });
    return $tr;
}

1;
