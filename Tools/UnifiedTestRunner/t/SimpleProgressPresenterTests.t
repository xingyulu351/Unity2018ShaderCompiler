use warnings;
use strict;

use Dirs;
use lib Dirs::external_root();
use Test::Output;
use Test::MockObject;
use Test::More;
use Test::MockModule;
use Test::Deep;
use Test::MockObject::Extends;
use Test::MockModule;

my $loggerModule;

BEGIN
{
    use_ok('ProgressPresenters::Simple');
    $loggerModule = new Test::MockModule('Logger');
    $loggerModule->mock(_isStdOutRedirected => sub { return 1; });
    $loggerModule->mock(_getConsoleCharWidth => sub { return 128; });
}

_buildMessage_StepDescriptionSpecified:
{
    my %loggerArgs = _stepStarted(description => 'Step1');
    cmp_deeply(\%loggerArgs, { msg => "Started: 'Step1'", resetLine => undef });
}

_buildMessage_DurationSpecified:
{
    my %loggerArgs = _stepStarted(description => 'Step1', duration => 123);
    cmp_deeply(\%loggerArgs, { msg => "Started: 'Step1' 0.123 seconds", resetLine => undef });
}

_buildMessage_ProgressSpecified:
{
    my %loggerArgs = _stepStarted(description => 'Step1', progress => { current => 1, total => 100 }, progressDetails => "doing something");
    cmp_deeply(\%loggerArgs, { msg => "Started: 'Step1' [1/100] doing something", resetLine => undef });
}

_buildMessage_ProgressSpecifiedAndNoDescription:
{
    my %loggerArgs = _stepStarted(progress => { current => 1, total => 100 }, progressDetails => "doing something");
    cmp_deeply(\%loggerArgs, { msg => "Started: [1/100] doing something", resetLine => undef });
}

_buildMessage_ResetLineSpecified:
{
    my %loggerArgs = _stepStarted(description => 'Step1', resetLine => 1);
    cmp_deeply(\%loggerArgs, { msg => "Started: 'Step1'", resetLine => 1 });
}

_buildMessage_Error:
{
    my %loggerArgs = _stepStarted(description => 'Step1', resetLine => undef, error => 'error text');
    cmp_deeply(\%loggerArgs, { msg => "Started: 'Step1' 'error text'", resetLine => undef });
}

_buildMessage_AllParamsSpecified:
{
    my %loggerArgs = _stepStarted(
        description => 'Step1',
        resetLine => 1,
        error => 'error text',
        progress => { current => 1, total => 100 },
        progressDetails => "doing something",
        error => 'error text'
    );
    cmp_deeply(\%loggerArgs, { msg => "Started: 'Step1' [1/100] 'error text' doing something", resetLine => 1 });
}

allMessagesHaveTheSameStructure:
{
    my $presenter = new ProgressPresenters::Simple();
    $presenter = new Test::MockObject::Extends($presenter);
    my %buildMessageArgs;
    $presenter->mock(
        _buildMessage => sub
        {
            (undef, undef, %buildMessageArgs) = @_;
        }
    );

    my $args = {
        description => "Test run",
        progress => { current => 1, total => 10 },
        progressDetails => 'progress details',
        resetLine => 1
    };

    $presenter->stepStarted(%{$args});
    cmp_deeply(\%buildMessageArgs, $args);
    $presenter->stepDone(%{$args});
    cmp_deeply(\%buildMessageArgs, $args);
    $presenter->stepError(%{$args});
    cmp_deeply(\%buildMessageArgs, $args);
}

sub _stepStarted
{
    my %args = @_;
    my $presenter = new ProgressPresenters::Simple();
    my $logger = new Test::MockModule('Logger');
    my %loggerArgs;
    $logger->mock(
        minimal => sub
        {
            %loggerArgs = @_;
        }
    );

    $presenter->stepStarted(%args);
    return %loggerArgs;
}

done_testing();
