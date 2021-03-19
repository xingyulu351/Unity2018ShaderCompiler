use warnings;
use strict;

use File::Temp qw (tempfile);

use Test::More;
use OSUtils;

use_ok('SystemCall');
use TestHelpers::ArrayAssertions qw (arraysAreEqual arrayContains);
use SystemCall qw (_quoteWinArgument);

executeAndWatchFiles_NoFiles_ExecutesCommand:
{
    my $systemCall = new SystemCall();
    my ($fh, $stdOutFile) = tempfile();
    close($fh);
    my $exitCode = $systemCall->executeAndWatchFiles(command => ['perl', '-e', '"print (\"line1\nline2\")"', "1>$stdOutFile"],);
    is($exitCode, 0);
}

executeAndWatchFiles_EmtptyFileArrray_ExecutesCommand:
{
    my $systemCall = new SystemCall();
    my ($fh, $stdOutFile) = tempfile();
    close($fh);
    my $filesToWatch = [];
    my $exitCode = $systemCall->executeAndWatchFiles(
        command => ['perl', '-e', '"print (\"line1\nline2\")"', "1>$stdOutFile"],
        filesToWatch => $filesToWatch
    );
    is($exitCode, 0);
}

executeAndWatchFiles_CanWatchOneFile:
{
    my $systemCall = new SystemCall();
    my @receivedLines : shared;
    my $callbackArg : shared;
    my $stdOutGrabber = sub
    {
        my ($line, $arg) = @_;
        $callbackArg = $arg;
        if (not defined($line))
        {
            return;
        }
        push(@receivedLines, $line);
    };

    my ($fh, $stdOutFile) = tempfile();
    close($fh);
    $systemCall->executeAndWatchFiles(
        command => ['perl', '-e', '"print (\"line1\nline2\")"', "1>$stdOutFile"],
        filesToWatch => [{ file => $stdOutFile, callback => $stdOutGrabber, callbackArg => 3 }]
    );
    arraysAreEqual(\@receivedLines, ['line1', 'line2']);
    is($callbackArg, 3);
}

executeAndWatchFiles_CanCaptureExitCode:
{
    my $systemCall = new SystemCall();
    my $exitCode = $systemCall->executeAndWatchFiles(
        command => ['perl', '-e', '"exit 123;"'],
        filesToWatch => []
    );
    is($exitCode, 123);
}

execute_CanCaptureExitCode:
{
    my $systemCall = new SystemCall();
    my $exitCode = $systemCall->execute('perl', '-e', '"exit 123;"');
    is($exitCode, 123);
}

if (OSUtils::isWindows())
{
    my $systemCall = new SystemCall();
    my $expectedExitCode = 3221225477;    # 0xC0000005
    my ($fh, $filename) = tempfile(SUFFIX => '.bat');
    print($fh "exit /b $expectedExitCode");
    close($fh);
    my $exitCode = $systemCall->execute($filename);
    is($exitCode, $expectedExitCode);
}

is(SystemCall::_quoteArgumentSwitch('foo'), 'foo');
is(SystemCall::_quoteArgumentSwitch('-switch'), '-switch');
is(SystemCall::_quoteArgumentSwitch('--switch'), '--switch');
is(SystemCall::_quoteArgumentSwitch('--switch=foo'), '--switch=foo');
is(SystemCall::_quoteArgumentSwitch('--switch=foo bar'), '"--switch=foo bar"');
is(SystemCall::_quoteArgumentSwitch('--switch=foo|bar'), '"--switch=foo|bar"');
is(SystemCall::_quoteArgumentSwitch('--switch=foo\"bar"&execute'), '"--switch=foo\\\\\\"bar\\"&execute"');
is(SystemCall::_quoteArgumentSwitch('-switch=foo bar'), '"-switch=foo bar"');
is(SystemCall::_quoteArgumentSwitch('-switch=foo|bar'), '"-switch=foo|bar"');
is(SystemCall::_quoteArgumentSwitch('1>/dev/nul'), '1>/dev/nul');
is(SystemCall::_quoteArgumentSwitch('<< input'), '<< input');
is(SystemCall::_quoteArgumentSwitch('--switch="already|quoted|value"'), '--switch="already|quoted|value"');
is(SystemCall::_quoteArgumentSwitch('--switch=foo\bar\baz'), '"--switch=foo\bar\baz"');
is(SystemCall::_quoteArgumentSwitch('--switch=foo\\"bar\\"baz\\'), '"--switch=foo\\\\\\"bar\\\\\\"baz\\\\"');

if (!OSUtils::isWindows())
{
    is(SystemCall::_quoteArgumentSwitch('--switch=foo\$bar$baz'), '"--switch=foo\\\\\\$bar\\$baz"');
}
else
{
    is(SystemCall::_quoteArgumentSwitch('--switch=foo\$bar$baz'), '"--switch=foo\\$bar$baz"');
}

done_testing();
