use warnings;
use strict;

use Test::More;
use Dirs;

use lib Dirs::external_root();
use Test::Trap;
use Test::Output;
use Test::MockObject;
use Test::MockModule;
use Term::ANSIColor qw(:constants);

my $fileUtilsMock;

BEGIN
{
    use_ok('Logger');
    $fileUtilsMock = new Test::MockModule('FileUtils');
    $fileUtilsMock->mock(readAllLines => sub { return (); });
    Logger::initialize('verbose');
}

my $loggerModule = new Test::MockModule('Logger');

$loggerModule->mock(_isStdOutRedirected => sub { return 1; });
$loggerModule->mock(_getConsoleCharWidth => sub { return 128; });

Verbose_NoSourceIsSpecified_PrintMessageInSpecifiedFormat:
{
    stdout_is(sub { Logger::verbose(msg => 'message'); }, "message\n");
}

Verbose_MessageSpecifiedAsSingleArgument_PrintMessageInSpecifiedFormat:
{
    stdout_is(sub { Logger::verbose('message'); }, "message\n");
}

Verbose_SourceIsSpecified_PrintMessageInSpecifiedFormat:
{
    stdout_is(sub { Logger::verbose(src => 'runner1', msg => 'message'); }, "[runner1] message\n");
}

Error_PrintMessageWithErrorPrefix:
{
    stderr_is(sub { Logger::error(src => 'runner1', msg => 'message'); }, "error: [runner1] message\n");
}

Minimal_PrintMessageInSpecifiedFormat:
{
    stdout_is(sub { Logger::minimal(msg => 'message'); }, "message\n");
}

Verbose_PrintsNothingIfLogLevelIsError:
{
    stdout_is(sub { Logger::raw(msg => "\nmessage\n"); }, "\nmessage\n");
}

FormatMessage_ReturnsMessageInSpecifiedFormat:
{
    my $result = Logger::formatMessage(src => 'runner1', msg => 'message');
    is($result, "[runner1] message\n");
}

PrintRedirectedMessage_CutsArtifactRoot:
{
    Logger::initialize('verbose');
    my $runContextMock = new Test::MockObject();
    $runContextMock->mock(getArtifactsPath => sub { return '/a/b/'; });
    Logger::setRunContext($runContextMock);
    stdout_is(sub { Logger::verbose(src => '/a/b/stdout.txt', msg => 'message'); }, "[stdout.txt] message\n");
}

Verbose_PrintsNothingIfLogLevelIsMinimal:
{
    Logger::initialize('minimal');
    stdout_is(sub { Logger::verbose(msg => 'message'); }, '');
}

Verbose_PrintsNothingIfLogLevelIsError:
{
    Logger::initialize('error');
    stdout_is(sub { Logger::verbose(msg => 'message'); }, '');
}

Error_ThatIsBlacklisted_DoesNotGetPrinted:
{
    $fileUtilsMock->mock(readAllLines => sub { return ('ApplePersistance=NO'); });
    Logger::initialize('error');
    stderr_is(sub { Logger::error(msg => 'ApplePersistance=NO'); }, '');
}

Error_ThatIsInMultipleBlacklistedItems_DoesNotGetPrinted:
{
    $fileUtilsMock->mock(readAllLines => sub { return ('ApplePersistance=NO', 'dylib error'); });
    Logger::initialize('error');
    stderr_is(sub { Logger::error(msg => 'RuntimeTests[92709:661485] dylib error'); }, '');
}

Error_PrintsNothingIfLogLevelIsNone:
{
    Logger::initialize('none');
    stderr_is(sub { Logger::verbose(msg => 'message'); }, '');
}

Warning_PrintsNothingIfLogLevelIsNone:
{
    Logger::initialize('none');
    stderr_is(sub { Logger::warning(msg => 'message'); }, '');
}

Warning_PrintsNothingIfLogLevelIsError:
{
    Logger::initialize('minimal');
    stderr_is(sub { Logger::warning(msg => 'message'); }, '');
}

Warning_PrintsMessageIfLogLevelIsError:
{
    Logger::initialize('verbose');
    stdout_is(sub { Logger::warning(msg => 'message'); }, "warning: message\n");
}

Error_PrintsNothingIfLogLevelIsNone:
{
    my @trap = trap
    {
        Logger::initialize('unknownlevel');
    };
    like($trap->die, qr/Unknown/);
}

consoleColorPrint:
{
    $loggerModule->mock(_isStdOutRedirected => sub { return 0; });
    Logger::initialize('verbose');
Error_PrintedInRed:
    {
        stderr_is(sub { Logger::error(src => 'runner1', msg => 'message'); }, "@{ [ RED ] }error: [runner1] message@{ [ RESET ] }\n");
    }

Warning_PrintedInYellow:
    {
        stdout_is(sub { Logger::warning(src => 'runner1', msg => 'message'); }, "@{ [ YELLOW ] }warning: [runner1] message@{ [ RESET ] }\n");
    }
}

done_testing();
