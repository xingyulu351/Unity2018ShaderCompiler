use warnings;
use strict;

use Test::More;
use SummaryPrinter;
use ResultCollector;
use Dirs;
use Logger;
use lib Dirs::external_root();
use Test::MockModule;
use Test::MockObject;
use Test::Differences;
use Term::ANSIColor qw(:constants);

my $loggerMock = new Test::MockModule('Logger');
$loggerMock->mock(_isStdOutRedirected => sub { return 1; });

my $runContextMock = new Test::MockObject();
$runContextMock->mock(
    getArtifactsPath => sub
    {
        return 'artifactsPath';
    }
);

RespectsTestMessageFormatting:
{
    $loggerMock->mock(_isStdOutRedirected => sub { return 1; });

    my $resultsCollector = new ResultCollector();
    my $sr = new SuiteResult(name => 'native', platform => 'editor');

    my $tr = new TestResult();
    $tr->setTest('test1');
    $tr->setState(TestResult::FAILURE);
    my $message = qq(CHECK FAILURE: Expected values to be the same, but they were not
        Expected: alamakota
          Actual: alamakotavfvggg

        (C:\\unity\\lane5\\Runtime/Core/Containers/StringBuilderTests.cpp:26)
);
    $tr->setMessage($message);
    $sr->addTestResult($tr);
    $resultsCollector->addSuiteResult($sr);

    my @output;
    $loggerMock->mock(
        minimal => sub
        {
            my ($line) = @_;
            push(@output, $line);
        }
    );

    SummaryPrinter::print($resultsCollector, $runContextMock);
    my $text = join("\n", @output);
    ok(index($text, $message) != -1);
}

HasCorrectReportAndColoringForSuccessfullRun:
{
    my $resultsCollector = new ResultCollector();
    my $sr = new SuiteResult(name => 'native', platform => 'editor');

    my $tr = new TestResult();
    $tr->setTest('test1');
    $tr->setState(TestResult::SUCCESS);
    $sr->addTestResult($tr);
    $resultsCollector->addSuiteResult($sr);

    my @output;
    $loggerMock->mock(
        minimal => sub
        {
            my ($line) = @_;
            push(@output, $line);
        }
    );

    my @lines = printSummary($resultsCollector, 0);
    is($lines[0], "Overall result: @{[ BRIGHT_GREEN ] }PASS@{ [ RESET ] }");
    is($lines[1], "Total Tests run: 1, Passed: 1, Failures: 0, Errors: 0, Inconclusives: 0");
    is($lines[2], "Total not run : 0, Invalid: 0, Ignored: 0, Skipped: 0");
}

HasCorrectReportAndColoringForFailedRun:
{
    my $resultsCollector = new ResultCollector();
    my $sr = new SuiteResult(name => 'native', platform => 'editor');

    my $tr1 = new TestResult();
    $tr1->setTest('test1');
    $tr1->setFixture('fixture1');
    $tr1->setState(TestResult::FAILURE);
    $tr1->setMessage('just failed');

    my $tr2 = new TestResult();
    $tr2->setTest('test2');
    $tr2->setState(TestResult::ERROR);
    $tr2->setMessage('just failed again');

    $sr->addTestResult($tr1);
    $sr->addTestResult($tr2);

    $resultsCollector->addSuiteResult($sr);

    my @output;
    $loggerMock->mock(
        minimal => sub
        {
            my ($line) = @_;
            push(@output, $line);
        }
    );

    my @lines = printSummary($resultsCollector, 0);
    is($lines[0], "Errors and Failures for native tests:");
    is($lines[1], "@{[ BRIGHT_RED ] }FAILURE: fixture1.test1@{[ RESET ] }");
    is($lines[2], "@{[ BRIGHT_RED ] }just failed@{[ RESET ] }");
    is($lines[3], "@{[ BRIGHT_RED ] }ERROR: test2@{[ RESET ] }");
    is($lines[4], "@{[ BRIGHT_RED ] }just failed again@{[ RESET ] }");
    is($lines[5], "Overall result: @{[ BRIGHT_RED ] }FAIL@{ [ RESET ] }");
    is($lines[6], "Total Tests run: 2, Passed: 0, Failures: @{[ BRIGHT_RED ] }1@{[ RESET ] }, Errors: @{[ BRIGHT_RED ] }1@{[ RESET ] }, Inconclusives: 0");
    is($lines[7], "Total not run : 0, Invalid: 0, Ignored: 0, Skipped: 0");
}

done_testing();

sub printSummary
{
    my ($resultsCollector, $stdOutRedirect) = @_;
    Logger::initialize('verbose');

    my @lines;
    $loggerMock->mock(
        minimal => sub
        {
            my ($line) = @_;
            push(@lines, $line);
        }
    );
    $loggerMock->mock(_isStdOutRedirected => sub { return $stdOutRedirect; });

    SummaryPrinter::print($resultsCollector, $runContextMock);
    return @lines;
}
