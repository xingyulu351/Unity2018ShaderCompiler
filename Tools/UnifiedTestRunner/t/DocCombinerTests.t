use warnings;
use strict;

use Test::More;
use TestHelpers::ArrayAssertions qw (arrayContains arraysAreEqual);
use TestHelpers::Runners qw (runAndGrabExecuteInput makeDefaultSuiteRunnerArtifactNameBuilderStub);
use Dirs;
use lib Dirs::external_root();
use Test::MockObject::Extends;
use Test::Deep 're';
use Test::MockModule;

BEGIN { use_ok('SuiteRunners::DocCombiner'); }

Creation:
{
    can_ok('SuiteRunners::DocCombiner', 'new');
    my $runner = new SuiteRunners::DocCombiner();
    isa_ok($runner, 'SuiteRunners::DocCombiner');
}

getName_ReturnsDocCombiner:
{
    my $runner = _makeRunner();
    is($runner->getName(), 'DocCombiner');
}

getPlatform_ReturnsDocCombiner:
{
    my $runner = _makeRunner();
    is($runner->getPlatform(), 'DocCombiner');
}

Run_DefaultArguments_ProducesCorectCommandLineAndArtifacts:
{
    my $runner = _makeRunner();

    my $nameBuilderStub = makeDefaultSuiteRunnerArtifactNameBuilderStub();

    $nameBuilderStub->specifyExpectation(
        method => 'getFullAtrifactFileName',
        params => { 'parent-type' => 'DocCombiner', type => 'stdout', extension => 'txt' },
        result => 'stdout_full_path'
    );

    $nameBuilderStub->specifyExpectation(
        method => 'getFullAtrifactFileName',
        params => { 'parent-type' => 'DocCombiner', type => 'stderr', extension => 'txt' },
        result => 'stderr_full_path'
    );

    $runner->setArtifactsNameBuilder($nameBuilderStub->object());

    my $fileWatcher = new Test::MockModule('FileWatcher');
    $fileWatcher->mock(watch => sub { });
    $fileWatcher->mock(start => sub { });
    $fileWatcher->mock(stop => sub { });
    my $jamWrapper = new Test::MockModule('JamWrapper');
    $jamWrapper->mock(Jam => sub { });

    my @commandLine = _run($runner);
    arraysAreEqual(\@commandLine, [re(qr/.*DocCombiner[\\\/]DocCombiner.*/), '-runNativeTests', '1>stdout_full_path', '2>stderr_full_path']);

    my @artifacts = $runner->getArtifacts()->get();
    arraysAreEqual(\@artifacts, ['stdout_full_path', 'stderr_full_path', re('DocCombiner')]);
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
    my $runner = new SuiteRunners::DocCombiner();
    $runner = new Test::MockObject::Extends($runner);
    my $nameBuilderStub = makeDefaultSuiteRunnerArtifactNameBuilderStub();
    $runner->setArtifactsNameBuilder($nameBuilderStub->object());
    $runner->mock(writeTestResultXml => sub { });
    $runner->mock(createArtifactsDirIfNotExistsOrCroak => sub { });
    $runner->mock(_getOutputBuildPath => sub { return 'build_path'; });

    return $runner;
}
