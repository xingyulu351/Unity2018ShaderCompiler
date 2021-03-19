package NativeBase;

use parent SuiteRunner;

use warnings;
use strict;

use File::Temp qw (tempfile);
use File::Spec::Functions qw (canonpath);
use Dirs;
use ProgressTracker;
use ResultPresenters::Simple ();
use FileUtils;
use English qw( -no_match_vars );

my $root = Dirs::getRoot();
my $xmlWriterCsProj = "$root/Tests/Unity.TestResultXmlWriter/Unity.TestResultXmlWriter.csproj";

use NativeSuiteOutputParser;
use SuiteRunnerArtifactsNameBuilder;

sub writeTemporaryTestFile
{
    my ($this, @testResults) = @_;
    my ($fh, $tempFileName) = tempfile();
    my $presenter = ResultPresenters::Simple->new();
    foreach (@testResults)
    {
        my $text = $presenter->present($_);
        print $fh "$text\n";
    }
    my $closeRes = close($fh);
    if (not $closeRes)
    {
        croak("Can not close the file $tempFileName. $OS_ERROR\n");
    }
    return $tempFileName;
}

sub buildDependencies
{
    my ($this) = @_;
    if ($this->{dependencies_built})
    {
        return;
    }

    $this->cleanCSharpProjectsSln();
    $this->buildCSProject($xmlWriterCsProj);

    $this->{dependencies_built} = 1;
}

sub getTestResults
{
    ## no critic (RequireBriefOpen)
    my ($this, $log_file) = @_;
    my $openRes = open(my $fh, '<', $log_file);
    if (!defined($openRes))
    {
        my @res = ();
        return @res;
    }
    my $parser = NativeSuiteOutputParser->new();
    $parser->setSuite($this->getName());
    while (<$fh>)
    {
        $parser->processLine($_);
    }

    my $closeRes = close($fh);
    if (not $closeRes)
    {
        croak("Can not open the file $log_file. $OS_ERROR\n");
    }

    my @results = $parser->getResults();
    foreach my $r (@results)
    {
        $r->setParent($this);
    }
    return @results;
}

sub postSuiteResult
{
    my ($this, $suiteResult) = @_;
    my $resultConsumer = $this->getResultConsumer();
    if (not defined($resultConsumer))
    {
        return;
    }
    $resultConsumer->addSuiteResult($suiteResult);
}

sub writeTestResultXml
{
    my ($this, @testResults) = @_;

    my $tempFileName = $this->writeTemporaryTestFile(@testResults);
    my @cmdArgs = ();
    push(@cmdArgs, "--suite=" . $this->getName());
    push(@cmdArgs, "--filewithtests=$tempFileName");
    my $testresultsxml = $this->testResultXmlFileName();
    push(@cmdArgs, "--testresultsxml=$testresultsxml");
    my $xmlWritterExe = $this->_getAssemblyFullPath($xmlWriterCsProj);
    $this->runDotNetProgram(
        program => $xmlWritterExe,
        programArgs => \@cmdArgs
    );
}

sub afterRun
{
    my ($this) = @_;
    my $logFile = $this->stdOutFileName();
    my @testResults = $this->getTestResults($logFile);
    $this->writeTestResultXml(@testResults);
}

sub testResultXmlFileName
{
    my ($this) = @_;
    return $this->testResultXmlFileNameImpl('UnifiedTestRunner');
}

sub startNewIteration
{
    my ($this) = @_;
    $this->{artifact_folders_prepared} = 0;
}

{    # printProgressMessage scope
    my $parser;
    my $currentTestNumber = 0;
    my $progressPresenter = undef;

    sub printProgressMessage
    {
        my ($line, $this) = @_;
        if (not defined($progressPresenter))
        {
            $progressPresenter = $this->getProgressPresenter();
            $parser = NativeSuiteOutputParser->new();
            $parser->setSuite($this->getName());
        }

        if (not defined($line))
        {
            return;
        }

        my $resultsCountBefore = $parser->getResultsCount();
        $parser->processLine($line);
        my $resultsCountAfter = $parser->getResultsCount();
        if ($resultsCountAfter > $resultsCountBefore)
        {
            ++$currentTestNumber;
            my @results = $parser->getResults();
            my $tr = $results[-1];
            my $testName = $tr->getTest();
            my $time = $tr->getTime();
            my $fixture = $tr->getFixture();
            my $state = $tr->getStateAsString();
            Logger::minimal("[$currentTestNumber/?] $fixture.$testName $state");
        }
    }
}

1;
