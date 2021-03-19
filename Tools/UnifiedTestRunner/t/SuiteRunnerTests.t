use warnings;
use strict;

use Test::More;

use RunContext;
use Dirs;
use lib Dirs::external_root();
use Test::MockObject::Extends;
use Test::MockModule;
use Environments::Mono;
use ArtifactsMap;
use ArtifactsMapEntry;

BEGIN { use_ok('SuiteRunner'); }

my $suiteOptions = [['suite_option:s', 'Option description', { defaultValue => 1 }]];

my $runnerOptionsLoaderMock = new Test::MockModule('RunnerOptions');
$runnerOptionsLoaderMock->mock(readFromFile => sub { return (); });

ExternalOptionsAreNotExposedByRunner:
{
    my $r = makeRunner($suiteOptions);
    is($r->getOptionValue('suite_option'), 1);
}

Run_UpdateCS_UpdatesHelpFile:
{
    my $runner = makeRunner();

    $runner->unmock('updateRunnerHelp');
    $runner->mock('buildCSProject2', sub { });
    $runner->mock('_generateMSBuildScript', sub { });

    my %runnerArgs;
    $runner->mock(runDotNetProgram => sub { shift; %runnerArgs = @_; });
    $runner->mock(_getAssemblyFullPath => sub { return 'runner.exe' });

    $runner->updateRunnerHelp();
    is($runnerArgs{program}, 'runner.exe');
}

RunnerOptionsAreExposedByRunner:
{
    my $runnerOptionsLoaderMock = new Test::MockModule('RunnerOptions');
    $runnerOptionsLoaderMock->mock(
        readFromFile => sub
        {
            my $runnerOptions = [['runner_option:s', 'Runner option', { defaultValue => 2 }]];
            return Option::fromDescriptions(@{$runnerOptions});
        }
    );

    my $r = makeRunner($suiteOptions);
    is($r->getOptionValue('runner_option'), 2);
}

ProtocolMessageEventIsInvoked:
{
    my $r = makeRunner($suiteOptions);
    my $msg = { type => 'dummy' };
    my $receivedMsg;
    $r->mock(
        onMessage => sub
        {
            my (undef, $m) = @_;
            $receivedMsg = $m;
        }
    );

    SuiteRunner::watchTestProtocolMessages('##utp:{"type":"dummy"}', $r);

    is_deeply($msg, $receivedMsg);
}

AddRunnerOptions_NotAddRunnerOptionsWithDefaultValues:
{
    my $runnerOptionsLoaderMock = new Test::MockModule('RunnerOptions');
    $runnerOptionsLoaderMock->mock(
        readFromFile => sub
        {
            my $runnerOptions = [['runner_option:s', 'Runner option', { defaultValue => 2 }]];
            return Option::fromDescriptions(@{$runnerOptions});
        }
    );

    my $r = makeRunner($suiteOptions);

    my @remainedOptions;
    $r->_addRunnerOptions(\@remainedOptions);
    is_deeply(\@remainedOptions, []);
}

ReportPerformanceDataMakesCorrectInvocation:
{
    my $performanceReporterMock = new Test::MockModule('PerformanceReporter');
    my $env;
    my %args;
    $performanceReporterMock->mock(
        report => sub
        {
            ($env, %args) = @_;
        }
    );

    my $r = makeRunner($suiteOptions);
    $r->setEnvironment(new Environments::Mono());
    $r->mock(getPathToReporter => sub { return 'path_to_reporter' });
    $r->mock(getPlatform => sub { return 'platform_name' });
    $r->mock(getName => sub { return 'name_value' });
    $r->mock(
        getOptionValue => sub
        {
            my (undef, $optionName) = @_;
            if ($optionName eq 'config')
            {
                return 'configName';
            }

            if ($optionName eq 'forcebaselineupdate')
            {
                return 1;
            }

            return undef;
        }
    );
    $r->mock(getReporterSuiteName => sub { return 'reporter_suite_name_value' });
    $r->mock(testResultXmlFileName => sub { return 'path_to_test_results_xml' });
    $r->mock(performanceDataFileName => sub { return 'path_to_output_json' });
    $r->mock(reporterStdOutFileName => sub { return 'path_to_reporter_stdout' });
    $r->mock(reporterStdErrFileName => sub { return 'path_to_reporter_stderr' });
    $r->_reportPerformanceData();
    isa_ok($env, 'Environment');
    is_deeply(
        \%args,
        {
            configName => 'configName',
            reporterPath => 'path_to_reporter',
            platform => 'platform_name',
            suiteName => 'reporter_suite_name_value',
            testResultsXml => 'path_to_test_results_xml',
            outputJson => 'path_to_output_json',
            csvResultsFile => undef,
            forceBaselineUpdate => 1,
            stdOut => 'path_to_reporter_stdout',
            stdErr => 'path_to_reporter_stderr',
        }
    );
}

Applies_ArtifactsMapToSuiteResults:
{
    my $r = makeRunner($suiteOptions);

    $r->mock(getArtifacts => sub { return new Artifacts(); });
    my $tr1 = new TestResult();
    $tr1->setTest('t1');
    my $tr2 = new TestResult();
    $tr2->setTest('t2');
    $r->mock(
        parseTestResultsXml => sub
        {
            return testResults => [$tr1, $tr2];
        }
    );

    $r->{runnerExitCode} = 0;

    $r->mock(
        getArtifactsMap => sub
        {
            my $artifactsMap = new ArtifactsMap();
            my $entry1 = new ArtifactsMapEntry('t1');
            $entry1->addArtifact('t1a1');
            $entry1->addArtifact('t1a2');

            my $entry2 = new ArtifactsMapEntry('t2');
            $entry2->addArtifact('t2a1');
            $entry2->addArtifact('t2a2');

            $artifactsMap->addTest($entry1);
            $artifactsMap->addTest($entry2);

            return $artifactsMap;
        }
    );

    my $sr = $r->createSuiteResult();
    my @testResults = $sr->getAllResults();

    my @t1Artifacts = $testResults[0]->getArtifacts();
    is_deeply(\@t1Artifacts, ['t1a1', 't1a2']);

    my @t2Artifacts = $testResults[1]->getArtifacts();
    is_deeply(\@t2Artifacts, ['t2a1', 't2a2']);
}

done_testing();

sub makeRunner
{
    my ($suiteOptions) = @_;
    my $r = new SuiteRunner(
        suiteOptions => $suiteOptions,
        optionsGenProj => 'runner.csproj'
    );

    $r = new Test::MockObject::Extends($r);
    $r->mock(
        getName => sub
        {
            return 'Fake';
        }
    );
    $r->mock(
        getPlatform => sub
        {
            return 'Fake';
        }
    );

    $r->mock(updateRunnerHelp => sub { return ('Options.gen.json', 1); });
    my $options = new Options(options => $r->getOptions());
    my $runContext = RunContext::new($options);
    $r->setRunContext($runContext);
    return $r;
}
