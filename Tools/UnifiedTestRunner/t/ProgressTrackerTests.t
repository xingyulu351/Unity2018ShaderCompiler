use warnings;
use strict;

use Dirs;
use lib Dirs::external_root();

use Test::More;
use Test::MockObject;
use TestHelpers::ArgsGrabber;

BEGIN: { use_ok('ProgressTracker') }

CREATION:
{
    can_ok('ProgressTracker', 'new');
    my $tracker = ProgressTracker->new(tracked_operation => 'Running Test Suite');
    is($tracker->getTrackedOperation(), 'Running Test Suite');
    isa_ok($tracker, 'ProgressTracker');
}

EXECUTION_STEP:
{
    my $tracker = ProgressTracker->new();
    can_ok($tracker, 'addStep');
    $tracker->addStep('Preparing Assets', 100);
    is($tracker->getTotalSteps(), 1);

    can_ok($tracker, 'incProgress');
    $tracker->incProgress('Preparing Assets', 2);
    my ($executed, $total) = $tracker->getStepProgress('Preparing Assets');
    is($executed, 2);
    is($total, 100);

    $tracker->addStep('Running Tests', 123);
    is($tracker->getTotalSteps(), 2);

    $tracker->incProgress('Preparing Assets', 1);
    $tracker->incProgress('Running Tests', 10);

    ($executed, $total) = $tracker->getStepProgress('Preparing Assets');
    is($executed, 3);
    is($total, 100);

    ($executed, $total) = $tracker->getStepProgress('Running Tests');

    is($executed, 10);
    is($total, 123);
}

UNKNOWN_UPPER_BOUND:
{
    my $tracker = ProgressTracker->new();
    $tracker->addStep('Step1', undef);
    $tracker->incProgress('Step1', 2);
    my ($executed, $total) = $tracker->getStepProgress('Step1');
    is($executed, 2);
    is($total, undef);
}

STEP_IS_AUTOMATICALLY_REGISTRED:
{
    my $tracker = ProgressTracker->new();
    $tracker->incProgress('Step1', 2);
    $tracker->incProgress('Step1', 2);
    is($tracker->getTotalSteps(), 1);
    my ($executed, $total) = $tracker->getStepProgress('Step1');
    is($executed, 4);
    is($total, undef);
}

PROGRESS_LISTENER_IS_NOTIFIED_ABOUT_START_OF_THE_STEP:
{
    my (@args);
    my $progressListener = createProgressListenerMock();
    $progressListener->mock(stepStarted => sub { shift(); push(@args, @_); });
    my $tracker = ProgressTracker->new(listener => $progressListener);
    $tracker->incProgress('Step', 10);
    is($args[0], 'Step');
    is($args[1], $tracker);
}

PROGRESS_LISTENER_IS_NOTIFIED_ABOUT_PROGRESS_CHANGE:
{
    my (@args);
    my $progressListener = createProgressListenerMock();
    $progressListener->mock(progressChange => sub { shift(); push(@args, @_); });
    my $tracker = ProgressTracker->new(listener => $progressListener);
    $tracker->incProgress('Step', 10, 'SomeTestMessage');
    is($args[0], 'Step');
    is($args[1], $tracker);
    is($args[2], 'SomeTestMessage');
}

STEP_STARTED_IS_CALLED_ONLY_ONCE:
{
    my $invokeCount = 0;
    my $progressListener = createProgressListenerMock();
    $progressListener->mock(stepStarted => sub { ++$invokeCount; });
    my $tracker = ProgressTracker->new(listener => $progressListener);
    $tracker->incProgress('Step', 10);
    $tracker->incProgress('Step', 20);
    $tracker->incProgress('Step', 30);
    is($invokeCount, 1);

    $invokeCount = 0;
    $tracker->addStep('Step2', 600);
    $tracker->incProgress('Step2', 10);
    $tracker->incProgress('Step2', 11);
    $tracker->incProgress('Step2', 12);
    is($invokeCount, 1);
}

STEP_DONE_IS_CALLED_ONLY_ONCE:
{
    my $invokeCount = 0;
    my $progressListener = createProgressListenerMock();
    $progressListener->mock(stepDone => sub { ++$invokeCount; });
    my $tracker = ProgressTracker->new(listener => $progressListener);

    $tracker->addStep('Step1', 600);
    $tracker->stepDone('Step1');
    $tracker->stepDone('Step1');

    is($invokeCount, 1);
}

ERRORS_COUNT:
{
    my $tracker = ProgressTracker->new();
    $tracker->addStep('Step');
    is($tracker->stepErrorsCount('Step'), 0);
    $tracker->addStepError('Step', 'SomeError');
    is($tracker->stepErrorsCount('Step'), 1);
}

STEP_DONE_AND_FINISHED:
{
    my $tracker = ProgressTracker->new();
    $tracker->addStep('Step');
    $tracker->stepDone('Step');
    is($tracker->stepErrorsCount('Step'), 0);
}

LISTENER_IS_NOTIFIED_ABOUT_INFO_MESSAGE:
{
    my $progressListener = createProgressListenerMock();
    my (@args);
    $progressListener->mock(info => sub { shift(); push(@args, @_); });

    my $tracker = ProgressTracker->new(listener => $progressListener);
    $tracker->info('message');

    is($args[0], 'message');
    is($args[1], $tracker);
}

LISTER_IS_NOTIFIED_ABOUT_ERROR:
{
    my $progressListener = createProgressListenerMock();

    my (@args);
    $progressListener->mock(stepError => sub { shift(); push(@args, @_); });

    my $tracker = ProgressTracker->new(listener => $progressListener);
    $tracker->addStepError('Step', 'Test 123 failed');

    is($args[0], 'Step');
    is($args[1], $tracker);
    is($args[2], 'Test 123 failed');
}

STEP_ORDER_NUMBER:
{
    my $tracker = ProgressTracker->new();
    $tracker->incProgress('Step1', 1);
    $tracker->incProgress('Step2', 2);
    $tracker->incProgress('Step3', 3);

    is($tracker->getStepOrder('Step3'), 3);
    is($tracker->getStepOrder('Step2'), 2);
    is($tracker->getStepOrder('Step1'), 1);
}

done_testing();

sub createProgressListenerMock
{
    my $progressListener = new Test::MockObject();
    $progressListener->mock(progressChange => sub { });
    $progressListener->mock(stepStarted => sub { });
    $progressListener->mock(stepDone => sub { });
    $progressListener->mock(info => sub { });
}
