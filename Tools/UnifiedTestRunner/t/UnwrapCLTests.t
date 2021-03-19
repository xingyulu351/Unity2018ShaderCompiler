use warnings;
use strict;

use Test::More;
use File::Spec::Functions qw (canonpath);
use TestHelpers::ArrayAssertions qw (arrayContains arrayDoesNotContain arraysAreEqual);
use TestHelpers::Runners qw (runAndGrabExecuteInput makeDefaultSuiteRunnerArtifactNameBuilderStub);
use lib Dirs::external_root();
use Test::Deep 're';
use File::Basename qw (dirname);

BEGIN { use_ok('SuiteRunners::UnWrapCL'); }

Creation:
{
    can_ok('SuiteRunners::UnWrapCL', 'new');
    my $runner = new SuiteRunners::UnWrapCL();
    isa_ok($runner, 'SuiteRunners::UnWrapCL');
}

getName_ReturnsUnWrapCL:
{
    my $runner = _makeRunner();
    is($runner->getName(), 'UnWrapCL');
}

getPlatform_ReturnsEditor:
{
    my $runner = _makeRunner();
    is($runner->getPlatform(), 'UnWrapCL');
}

Run_DefaultArguments_ProducesCorectCommandLineAndArtifacts:
{
    my $runner = _makeRunner();

    my $nameBuilderStub = makeDefaultSuiteRunnerArtifactNameBuilderStub();

    $nameBuilderStub->specifyExpectation(
        method => 'getFullAtrifactFileName',
        params => { 'parent-type' => 'UnWrapCL', type => 'stdout', extension => 'txt' },
        result => 'stdout_full_path'
    );

    $nameBuilderStub->specifyExpectation(
        method => 'getFullAtrifactFileName',
        params => { 'parent-type' => 'UnWrapCL', type => 'stderr', extension => 'txt' },
        result => 'stderr_full_path'
    );

    $nameBuilderStub->specifyExpectation(
        method => 'getFullAtrifactFileName',
        params => { 'parent-type' => 'UnifiedTestRunner', type => 'TestResults', extension => 'xml' },
        result => 'test_results_xml_full_file_name'
    );

    $runner->setArtifactsNameBuilder($nameBuilderStub->object());

    my @commandLine = _run($runner);
    arraysAreEqual(\@commandLine, [re(qr/.*Tools[\\\/]UnwrapCL.*/), '-runNativeTests', '1>stdout_full_path', '2>stderr_full_path']);

    my @artifacts = $runner->getArtifacts()->get();
    arraysAreEqual(\@artifacts, ['stdout_full_path', 'stderr_full_path']);
}

Run_TestResultsXmlIsSpecified_ReportsItIntoArtifacts:
{
    my $runner = _makeRunner();
    my $nameBuilderStub = makeDefaultSuiteRunnerArtifactNameBuilderStub();

    $nameBuilderStub->specifyExpectation(
        method => 'getFullAtrifactFileName',
        params => { 'parent-type' => 'UnifiedTestRunner', type => 'TestResults', extension => 'xml' },
        result => 'test_results_xml_full_file_name'
    );

    $runner->setArtifactsNameBuilder($nameBuilderStub->object());
    _run($runner, '--testresultsxml=path_to_testresults_xml');

    my @artifacts = $runner->getArtifacts()->get();
    arrayDoesNotContain(\@artifacts, 'path_to_testresults_xml');
}

Run_DefaultArguments_InvokesChangeDir:
{
    my $runner = _makeRunner();
    my $chDirCommand;
    $runner->mock(
        '_pushd',
        sub
        {
            my ($this, $dir) = @_;
            $chDirCommand = $dir;
        }
    );

    my @commandLine = _run($runner);

    is($chDirCommand, dirname($commandLine[0]));
}

done_testing();

sub _run
{
    my ($runner, @args) = @_;
    if (not defined($runner))
    {
        $runner = _makeRunner();
    }
    return runAndGrabExecuteInput($runner, @args);
}

sub _makeRunner
{
    my $runner = new SuiteRunners::UnWrapCL();
    $runner = new Test::MockObject::Extends($runner);
    my $nameBuilderStub = makeDefaultSuiteRunnerArtifactNameBuilderStub();
    $runner->setArtifactsNameBuilder($nameBuilderStub->object());
    $runner->mock(writeTestResultXml => sub { });
    $runner->mock(_getOutputBuildPath => sub { return 'build_path'; });
    $runner->mock(_pushd => sub { });
    return $runner;
}
