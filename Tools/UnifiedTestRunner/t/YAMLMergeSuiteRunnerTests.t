use warnings;
use strict;

use Test::More;
use SuiteRunners::YAMLMerge;
use File::Spec::Functions qw (canonpath);
use TestHelpers::Runners qw (runAndGrabExecuteInput makeDefaultSuiteRunnerArtifactNameBuilderStub);
use TestHelpers::ArrayAssertions qw (arraysAreEqual arrayDoesNotContain arrayContains arrayStartsWith);

use lib Dirs::external_root();
use Test::MockObject::Extends;

Run_RequiredArguments_Produces_CorrectCommandLineAndArtifacts:
{
    my $runner = makeRunner();
    my $nameBuilderStub = makeDefaultSuiteRunnerArtifactNameBuilderStub();

    $nameBuilderStub->specifyExpectation(
        method => 'getFullAtrifactFileName',
        params => { 'parent-type' => 'UnityYAMLMerge', type => 'stdout', extension => 'txt' },
        result => 'stdout_full_path'
    );

    $nameBuilderStub->specifyExpectation(
        method => 'getFullAtrifactFileName',
        params => { 'parent-type' => 'UnityYAMLMerge', type => 'stderr', extension => 'txt' },
        result => 'stderr_full_path'
    );

    $runner->setArtifactsNameBuilder($nameBuilderStub->object());

    my @input = runAndGrabExecuteInput($runner);
    is(scalar(@input), 4);

    my $executable = TargetResolver::resolve('yamlmerge');

    arrayStartsWith(\@input, ['perl', $executable, '1>stdout_full_path', '2>stderr_full_path']);

    my @artifacts = $runner->getArtifacts()->get();
    arraysAreEqual(\@artifacts, ['stdout_full_path', 'stderr_full_path',]);
}

Run_TestResultsXmlIsSpecified_ReportsItIntoArtifacts:
{
    my $runner = makeRunner();

    runAndGrabExecuteInput($runner, '--testresultsxml=tr.xml');

    my @artifacts = $runner->getArtifacts()->get();
    arrayDoesNotContain(\@artifacts, canonpath('tr.xml'));
}

Run_ModeIsSpecified_PutsItToCommandLine:
{
    my $runner = makeRunner();
    my $nameBuilderStub = makeDefaultSuiteRunnerArtifactNameBuilderStub();

    $runner->setArtifactsNameBuilder($nameBuilderStub->object());

    my @input = runAndGrabExecuteInput($runner, '--mode=native');
    arrayContains(\@input, 'native');
}

Run_TestResultsXmlIsSpecified_ReportsItIntoArtifacts:
{
    my $runner = makeRunner();

    runAndGrabExecuteInput($runner, '--testresultsxml=TestResults.xml');

    my @artifacts = $runner->getArtifacts()->get();
    arrayDoesNotContain(\@artifacts, canonpath('TestResults.xml'));
}

done_testing();

sub makeRunner
{
    my $runner = new SuiteRunners::YAMLMerge();
    $runner = new Test::MockObject::Extends($runner);
    $runner->mock(writeTestResultXml => sub { });
    return $runner;
}
