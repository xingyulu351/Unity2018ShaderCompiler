#!/usr/bin/perl

use File::Basename qw (dirname);

use warnings;
use strict;

use Carp qw (croak);
use Cwd;

use File::Spec;
use File::Copy;

use File::Spec::Functions qw (catfile catdir);
use lib File::Spec->rel2abs(dirname($0));
use Dirs;
use lib Dirs::external_root();
use File::Copy::Recursive qw(dircopy);
use Suites;
use SuiteRunners;
use ProgressPresenters;
use NativeBase;
use RunContext;
use ResultPresenters::Simple;
use ProgressPresenters::Simple;
use ProgressPresenters::Null;
use ResultCollector;
use GlobalOptions;
use SystemCall;

use Artifacts;
use SummaryPrinter;
use StringUtils;
use SingleSuiteRunnerArtifactsNameBuilder;
use SuiteRunnerArtifactsNameBuilder;

use JSON;
use ResultSubmitter;
use Hoarder::Collectors;
use Hoarder::TestSessionSubmissionRequestBuilder;
use Logger;
use Report::TestSessionReportBuilder;
use FileUtils;
use HtmlReportGenerator;
use HelpPrinter;
use Options;
use Printers::Console;
use ExtendedProfileReader;
use ArgumentNamesExtractor;
use AppHelper;
use RemoteEventLogger;
use UnityTestProtocol::Messages;
use File::Path qw (mkpath);
use UtrUsage;

my $runContext = undef;
my $resultCollector = ResultCollector::new();
my $agregatedResultsCollector = ResultCollector::new();
my $unknownSuite = undef;
my $root = Dirs::getRoot();

BEGIN { $| = 1 }

my $startTime;

sub main
{
    $ENV{'UNITY_UNIFIED_TEST_RUNNER_RUNNER'} = '1';
    $startTime = time();
    @ARGV = map {
        $_ =~ s/platform=iphone/platform=ios/g;
        $_ =~ s/platform=StandaloneOSX(Intel|Intel64|Universal)$/platform=StandaloneOSX/g;
        $_;
    } @ARGV;

    my $result = AppHelper::parseOptions(@ARGV);

    my $options = $result->{options};
    if ($options->getOptionValue('nocolor'))
    {
        $ENV{ANSI_COLORS_DISABLED} = 1;
    }
    if ($result->{expandedCommandLine})
    {
        @ARGV = @{ $result->{expandedCommandLine} };
    }

    $ENV{'UNITY_UNIFIED_TEST_COMMAND_LINE'} = objToJson(\@ARGV);
    $runContext = RunContext::new($options);
    initLogger($options);
    my $reportGenerationTest = $options->getOptionValue('report-generation-test');
    if ($reportGenerationTest)
    {
        my $artifactsPath = $options->getOptionValue("artifacts_path");
        my @args = ('perl', catfile(dirname($0), 'GenFakeReport.pl'), "--artifacts_path=$artifactsPath");
        my $partitionIndex = $options->getOptionValue("partition-index");
        if ($partitionIndex)
        {
            push(@args, "--partition-index=" . $partitionIndex);
        }
        system(@args);
        exit(0);
    }

    Logger::setRunContext($runContext);
    printHelpIfRequestedAndExit($options);
    printUnkownOptionsIfAny($options);
    _verifyIncludedLibs();
    if ($options->getOptionValue('build-only'))
    {
        build($options);
    }
    elsif ($options->getOptionValue('list'))
    {
        list($options);
    }
    elsif ($options->getOptionValue('update-runner-options'))
    {
        updateRunnerHelp($options);
        doExitWithCode(0);
    }
    elsif ($options->getOptionValue('query-partitions'))
    {
        my $result = queryParitions($options);
        if (not $result)
        {
            Logger::error("Can not query paritions. See previous errors");
            doExitWithCode(1);
        }
        my $partitionsJsonFilePath = $options->getOptionValue('query-partitions-result');
        if (not $partitionsJsonFilePath)
        {
            $partitionsJsonFilePath = catfile($options->getOptionValue('artifacts_path'), 'paritions.json');
        }
        my $partitionsJsonDirPath = FileUtils::getDirName($partitionsJsonFilePath,);
        if (not -d $partitionsJsonDirPath)
        {
            if (not mkpath($partitionsJsonDirPath))
            {
                croak("Can not create dir: $partitionsJsonDirPath");
            }
        }
        FileUtils::writeAllLines($partitionsJsonFilePath, $result);
        Logger::minimal("Partitions JSON file has been successfully written to $partitionsJsonFilePath");
        doExitWithCode(0);
    }
    else
    {
        initRemoteLogger($options);
        my $msg = UnityTestProtocol::Messages::makeSessionStartMessage();
        RemoteEventLogger::log($msg);
        my $extendedProfile = $options->getOptionValue('runrules');
        if ($extendedProfile)
        {
            runWithExtendedProfile($extendedProfile, $options);
        }
        else
        {
            run($options);
        }
    }

    doExit($options);
}

sub printHelpIfRequestedAndExit
{
    my ($options) = @_;
    if (not $options->getOptionValue('help'))
    {
        return;
    }
    my @runners = AppHelper::createRunners($options);
    printHelp(@runners);
    exitTestSucceeded();
}

sub printHelp
{
    my (@plugins) = @_;
    if (@plugins)
    {
        foreach my $p (@plugins)
        {
            my %params = $p->usage();
            $params{name} = $p->getName();
            my @options = $p->getOptionsWithoutNamespace();
            $params{options} = \@options;
            my $text = SuiteHelpPrinter::getText(%params);
            Printers::Console::printRaw($text . "\n");
        }
        return;
    }

    my @globalOptions = GlobalOptions::getOptions();
    my %usage = UtrUsage::get();
    $usage{options} = \@globalOptions;
    my $text = HelpPrinter::getText(%usage);
    Printers::Console::printRaw("$text\n");
}

sub queryParitions
{
    my ($options) = @_;
    my @runners = AppHelper::createRunners($options);
    if (scalar(@runners) == 0)
    {
        Logger::error("No test suites have been selected to run. Type ./utr.pl --help for more information");
        return;
    }

    #todo: restrict only one runner
    my $runner = $runners[0];
    return $runner->queryPartitions();
}

sub run
{
    my ($options) = @_;

    my @runners = AppHelper::createRunners($options);
    if (scalar(@runners) == 0)
    {
        Logger::error("No test suites have been selected to run. Type ./utr.pl --help for more information");
        return;
    }
    runSpecifiedSuites($options, @runners);
    my $repeat = $options->getOptionValue('repeat');
    if ($repeat > 1)
    {
        SummaryPrinter::print($agregatedResultsCollector, $runContext);
    }

    submitResults($agregatedResultsCollector, $options);
    generateExecutionReport($agregatedResultsCollector);
}

sub runSpecifiedSuites
{
    my ($options, @runners) = @_;
    activateDebugIfSpecified($options);
    my $repeat = $options->getOptionValue('repeat');
    my $existingIterationPrefix = $runContext->getIteration();
    for (my $i = 0; $i < $repeat; ++$i)
    {
        $resultCollector = new ResultCollector();
        if ($repeat > 1)
        {
            if ($existingIterationPrefix)
            {
                $runContext->setIteration("$existingIterationPrefix-$i");
            }
            else
            {
                $runContext->setIteration($i);
            }
        }
        foreach my $runner (@runners)
        {
            runSuite($runner, $options, scalar(@runners));
        }

        my %summary = buildSummary($resultCollector);
        SummaryPrinter::print($resultCollector, $runContext);
        saveSuiteRunsToAggregatedResult($resultCollector);
        printArtifacts(@runners);
    }
}

sub saveSuiteRunsToAggregatedResult
{
    my ($resultCollector) = @_;
    foreach my $suiteResult ($resultCollector->getAllSuiteResults())
    {
        $agregatedResultsCollector->addSuiteResult($suiteResult);
    }
}

sub runSuite
{
    my ($runner, $options, $totalSuites) = @_;
    $runner->setResultConsumer($resultCollector);
    $runner->setRunContext($runContext);
    $runner->setArtifacts(new Artifacts());

    if ($totalSuites == 1)
    {
        $runner->setArtifactsNameBuilder(new SingleSuiteRunnerArtifactsNameBuilder($runner));
    }
    else
    {
        $runner->setArtifactsNameBuilder(new SuiteRunnerArtifactsNameBuilder($runner));
    }

    $runner->startNewIteration();

    my $environment = AppHelper::createEnvironment($options);
    $runner->setEnvironment($environment);
    my $progressPresenter = getProgressPresenter($options);
    $runner->setProgressPresenter($progressPresenter);
    $runner->run();
}

sub list
{
    my ($options) = @_;
    my @runners = AppHelper::createRunners($options);
    my @all_tests = ();
    foreach my $runner (@runners)
    {
        my @tests = listRunnerTests($runner, $options);
        push(@all_tests, @tests);
    }
    foreach my $test (@all_tests)
    {
        print("$test\n");
    }
}

sub listRunnerTests
{
    my ($runner, $options) = @_;
    $runner->setRunContext($runContext);
    my $environment = AppHelper::createEnvironment($options);
    $runner->setEnvironment($environment);
    $runner->setArtifactsNameBuilder(SuiteRunnerArtifactsNameBuilder->new($runner));
    return $runner->list();
}

sub build
{
    my ($options) = @_;
    my @runners = AppHelper::createRunners($options);
    foreach my $runner (@runners)
    {
        $runner->setArtifacts(new Artifacts());
        $runner->buildPlayer();
    }
    printArtifacts(@runners);

}

sub createTool
{
    my ($options) = @_;
    my $toolname = $options->getOptionValue('tool');
    return SuiteTools::getTool($toolname);
}

sub getProgressPresenter
{
    my ($options) = @_;
    my $progressTrackerType = $runContext->getOptionValue('simple', 'progress');
    my $progressPresenter = ProgressPresenters::getProgressPresenter($progressTrackerType);
    my $loglevel = getLogLevel($options);
    if (not defined($progressPresenter))
    {
        croak("Unknown progress tracker type: $progressTrackerType");
    }
    return $progressPresenter;
}

sub activateDebugIfSpecified
{
    my ($options) = @_;
    if (not defined($options->getOptionValue("debug")))
    {
        return;
    }
    $ENV{'UNITY_GIVE_CHANCE_TO_ATTACH_DEBUGGER'} = 1;
}

sub printArtifacts
{
    my (@runners) = @_;
    for my $runner (@runners)
    {
        my $suite = $runner->getName();
        my @runArtifacts = $runner->getArtifacts()->get($suite);
        if (!@runArtifacts)
        {
            return;
        }
        Logger::minimal("$suite suite artifacts:");
        foreach my $a (@runArtifacts)
        {
            Logger::minimal("\t$a");
        }
    }
}

sub submitResults
{
    my ($resultCollector, $options) = @_;

    my $jsonReportPath = catfile($runContext->getArtifactsPath(), "TestReportData.json");
    my %summary;
    if (-e $jsonReportPath)
    {
        my $testReport = FileUtils::readJsonObj($jsonReportPath);
        %summary = %{ $testReport->{summary} };
        $summary{success} = $summary{success} ? 1 : 0;
    }
    else
    {
        %summary = SummaryBuilder::build($resultCollector->getAllSuiteResults());
    }

    my $summaryMsg = UnityTestProtocol::Messages::makeSessionEndMessage(\%summary);
    RemoteEventLogger::log($summaryMsg);
    my $hoarderUri = getHoarderUri($options);

    #TODO: check if not source code customer
    if (not defined($hoarderUri))
    {
        return $hoarderUri;
    }
    my @collectors = Hoarder::Collectors::getAllCollectors();
    my @suiteResults = $resultCollector->getAllSuiteResults();
    my $requestBuilder = new Hoarder::TestSessionSubmissionRequestBuilder();
    my $request = $requestBuilder->build(
        collectors => \@collectors,
        suite_results => \@suiteResults,
        smartSelectSessionId => $runContext->getSmartSelectSessionId()
    );
    my %data = %{ $request->data() };
    my $json = JSON->new(utf8 => 1, convblessed => 1);
    my $jsonText = $json->to_json(\%data);
    my $sessionDataFile = prepareSessionDataFile($jsonText);
    my $submitter = ResultSubmitter->new(
        'system' => SystemCall->new(),
        'hoarder-uri' => $hoarderUri,
        'artifacts-root' => $runContext->getArtifactsPath(),
    );

    my (@referencedData);
    foreach my $sr (@suiteResults)
    {
        if ($sr->getTestResultXmlFilePath())
        {
            push(@referencedData, $sr->getTestResultXmlFilePath());
        }
    }
    my $format = 'json_header/refrenced_data_v1';
    if (scalar(@referencedData) eq 0)
    {
        $format = 'json';
    }
    $submitter->submit($format, $sessionDataFile, @referencedData);
}

sub generateExecutionReport
{
    my @suiteResults = $agregatedResultsCollector->getAllSuiteResults();
    my @collectors = Hoarder::Collectors::getAllCollectors();
    my @commandLine = @ARGV;

    #reset iterations, to return back to the original artifacts path
    $runContext->setIteration(undef);
    my $artifactsRoot = $runContext->getArtifactsPath();

    HtmlReportGenerator::generate(
        suiteResults => \@suiteResults,
        collectors => \@collectors,
        artifactsRoot => $artifactsRoot,
        commandLine => \@commandLine
    );
}

sub getHoarderUri
{
    my ($options) = @_;
    if (exists($ENV{'UNITY_HOARDER_URI'}) and not defined($ENV{'UNITY_HOARDER_URI'}))
    {
        return undef;    #no need to report
    }

    my $hoarderUri = $options->getOptionValue('hoarder-uri');
    if (not defined($hoarderUri))
    {
        $hoarderUri = "http://api.hoarder.qa.hq.unity3d.com/v1/utr";
        if (exists($ENV{'UNITY_HOARDER_URI'}) and defined($hoarderUri))
        {
            $hoarderUri = $ENV{'UNITY_HOARDER_URI'};
        }
    }

    return $hoarderUri;
}

sub prepareSessionDataFile
{
    my ($jsonText) = @_;
    my $jsonFile = catfile($runContext->getArtifactsPath(), "test_session_data.txt");
    open(my $fh, '>', $jsonFile);
    print($fh $jsonText);
    close($fh);
    return $jsonFile;
}

sub exitListSucceeded
{
    doExitWithCode(0);
}

sub exitTestSucceeded
{
    doExitWithCode(0);
}

sub exitTestFailed
{
    doExitWithCode(1);
}

sub exitAbort
{
    doExitWithCode(255);
}

sub exitDidntRunTests
{
    doExitWithCode(1);
}

sub doExit
{
    my ($options) = @_;
    if ($options->getOptionValue('build-only'))
    {
        doExitWithCode(0);
    }
    elsif ($options->getOptionValue("list"))
    {
        doExitWithCode(0);
    }

    my %summary = buildSummary($agregatedResultsCollector);
    if (not $summary{success})
    {
        exitTestFailed();
    }

    exitTestSucceeded();
}

sub buildSummary
{
    my ($resultCollector) = @_;
    my $testReport = catfile($runContext->getArtifactsPath(), "TestReportData.json");
    if (-e $testReport)
    {
        my $report = FileUtils::readJsonObj($testReport);
        return %{ $report->{summary} };
    }

    my @suiteResults = $resultCollector->getAllSuiteResults();
    for my $sr (@suiteResults)
    {
        if (not $sr->getZeroTestsAreOk())
        {
            my %suiteSummary = SummaryBuilder::build($sr);
            if ($suiteSummary{hasSuiteWithNoTests})
            {
                my $suite = $sr->getName();
                if ($sr->getDescription())
                {
                    $suite .= "/" . $sr->getDescription();
                }
                $sr->forceFailed("$suite: no tests has been selected to run. to change this behaviour use '--zero-tests-are-ok=1'");
            }
        }

        if (not $sr->isForcibilyFailed())
        {
            next;
        }

        my @reasons = $sr->getFailureReasons();
        foreach my $r (@reasons)
        {
            Logger::error($r);
        }
    }

    my %summary = SummaryBuilder::build(@suiteResults);
    return %summary;
}

sub doExitWithCode
{
    my ($code) = @_;
    my $duration = time() - $startTime;
    Logger::minimal("Unified Test Runner exiting with code $code. Duration: $duration seconds");
    RemoteEventLogger::shutdown();
    exit($code);
}

sub getLogLevel
{
    my ($options) = @_;

    my $loglevel = $options->getOptionValue('loglevel');
    if (not defined($loglevel))
    {
        $loglevel = 'minimal';
    }
    return $loglevel;
}

sub initLogger
{
    my ($options) = @_;
    my $loglevel = getLogLevel($options);
    Logger::initialize($loglevel);
}

sub initRemoteLogger
{
    my ($options) = @_;
    RemoteEventLogger::initialize(
        owner => $options->getOptionValue('owner'),
        uri => $options->getOptionValue('event-uri'),
    );
}

sub printUnkownOptionsIfAny
{
    my ($options) = @_;
    if (not $options)
    {
        return;
    }
    my @unknownOptions = $options->getUnknownOptions();
    if (scalar(@unknownOptions) > 0)
    {
        Logger::warning('Ignoring unknown options: ' . join(' ', @unknownOptions));
    }
}

sub runWithExtendedProfile
{
    my ($eprofFileName, $options) = @_;
    my @argvCopy = @ARGV;
    my $eprof = ExtendedProfileReader::read($eprofFileName, \@argvCopy);
    my @suites = $eprof->getSuites();
    $runContext = RunContext::new($options);
    my $i = 0;
    foreach my $suite (@suites)
    {
        @ARGV = ();

        push(@ARGV, "--suite=" . $suite->getName());

        my @combinedArgs = $suite->getCombinedArgs();
        @combinedArgs = grep { $_ !~ qr/runrules=/ } @combinedArgs;
        push(@ARGV, @combinedArgs);

        my $result = AppHelper::parseOptions(@ARGV);
        $options = $result->{options};
        @ARGV = @{ $result->{expandedCommandLine} };
        $runContext->{options} = $options;
        $runContext->setIteration("run" . ++$i);
        my $runner = SuiteRunners::getRunner($suite->getName());
        runSpecifiedSuites($options, $runner);
        my @suiteResults = $agregatedResultsCollector->getAllSuiteResults();
        my $lastSuiteResult = $suiteResults[-1];

        if ($lastSuiteResult)
        {
            $lastSuiteResult->setDetails($suite->getDetails());
            $lastSuiteResult->setMinimalCommandLine(@ARGV);
        }
    }
    $runContext->setIteration(undef);
    SummaryPrinter::print($agregatedResultsCollector, $runContext);
    submitResults($agregatedResultsCollector, $options);
    generateExecutionReport($agregatedResultsCollector);
}

sub updateRunnerHelp
{
    my ($options) = @_;
    my @runners = AppHelper::createRunners($options);
    foreach my $runner (@runners)
    {
        my (undef, $runnerExitCode) = $runner->updateRunnerHelp();
        if ($runnerExitCode != 0)
        {
            croak("Failed to update help for " . $runner->getName());
        }
    }
}

$SIG{__DIE__} = sub
{
    my $inEval = not(defined($^S) and $^S == 0);
    if ($inEval)
    {

        # see http://perldoc.perl.org/perlvar.html#%24EXCEPTIONS_BEING_CAUGHT
        # We're in an eval {} and don't want log
        # this message but catch it later
        return;
    }

    Logger::fatal(@_);

    die("program terminated");
};

#From time to time people are including  something to UTR which points to External/Perl/lib/
#wich could cause issues because there are tons of libraries with different version
#Recent example is that Perl 5.16.2 contain this bug https://github.com/libwww-perl/net-http/pull/11
#so we don't want @INC to contain anything pointing to External/Perl/lib/
sub _verifyIncludedLibs
{
    foreach my $inc (@INC)
    {
        if (   $inc =~ m/External[\\\/]+Perl[\\\/]+lib/i
            or $inc =~ m/Tools[\\\/]+Build/i
            or $inc =~ m/Tools[\\\/]+SDKDownloader/i)
        {
            _croakWithInvalidInclude($inc);
        }
    }
}

sub _croakWithInvalidInclude
{
    my ($inc) = @_;
    croak("UTR must not include anything from $inc. Contact #qa-frameworks");
}

main();
