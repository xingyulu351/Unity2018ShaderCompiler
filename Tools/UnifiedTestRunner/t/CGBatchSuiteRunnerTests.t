use warnings;
use strict;

use File::Spec::Functions qw (canonpath);
use TestHelpers::Runners qw (runAndGrabExecuteInput makeDefaultSuiteRunnerArtifactNameBuilderStub);
use TestHelpers::ArrayAssertions qw (arrayStartsWith arrayDoesNotContain arrayContains arrayEndsWith arraysAreEqual);
use TestHelpers::Runners;
use TargetResolver;
use Test::More;

use lib Dirs::external_root();
use Test::Deep 're';
use Test::MockObject::Extends;

BEGIN { use_ok('SuiteRunners::CGBatch'); }

BASIC:
{
    can_ok('SuiteRunners::CGBatch', 'new');
    my $po = SuiteRunners::CGBatch->new();
    isa_ok($po, 'SuiteRunners::CGBatch');
    can_ok('SuiteRunners::CGBatch', 'run');
}

Run_RequiredArguments_Produces_CorrectCommandLineAndArtifacts:
{
    my $runner = makeRunner();
    my $nameBuilderStub = makeDefaultSuiteRunnerArtifactNameBuilderStub();

    $nameBuilderStub->specifyExpectation(
        method => 'getFullAtrifactFileName',
        params => { 'parent-type' => 'UnityShaderCompiler', type => 'stdout', extension => 'txt' },
        result => 'stdout_full_path'
    );

    $nameBuilderStub->specifyExpectation(
        method => 'getFullAtrifactFileName',
        params => { 'parent-type' => 'UnityShaderCompiler', type => 'stderr', extension => 'txt' },
        result => 'stderr_full_path'
    );

    $runner->setArtifactsNameBuilder($nameBuilderStub->object());
    my @input = runAndGrabExecuteInput($runner, '--suite=cgbatch');
    is(scalar(@input), 4);

    arrayStartsWith(\@input, [re(qr/.*Tools[\\\/]UnityShaderCompiler.*/), '-runNativeTests', '1>stdout_full_path', '2>stderr_full_path']);

    my @artifacts = $runner->getArtifacts()->get();
    arraysAreEqual(\@artifacts, ['stdout_full_path', 'stderr_full_path',]);
}

Run_TestResultsXmlIsSpecified_ReportsItIntoArtifacts:
{
    my $runner = makeRunner();

    runAndGrabExecuteInput($runner, '--testresultsxml=tr.xml');

    my @artifacts = $runner->getArtifacts()->get();
    is(scalar(@artifacts), 2);

    arrayDoesNotContain(\@artifacts, canonpath('tr.xml'));
}

Run_FilterSpecified_PutItIntoResultCommandLine:
{
    my @input = run('--testfilter=Everything');
    arrayContains(\@input, 'Everything');
}

done_testing();

sub makeRunner
{
    my $runner = new SuiteRunners::CGBatch();
    $runner = new Test::MockObject::Extends($runner);
    my $nameBuilderStub = makeDefaultSuiteRunnerArtifactNameBuilderStub();
    $runner->setArtifactsNameBuilder($nameBuilderStub->object());
    $runner->mock(writeTestResultXml => sub { });
    $runner->mock(_getOutputBuildPath => sub { return 'build_path'; });
    return $runner;
}

sub run
{
    my (@args) = @_;
    my $runner = makeRunner();
    return runAndGrabExecuteInput($runner, @args);
}
