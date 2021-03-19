package SystemCall;

use warnings;
use strict;

use Carp qw (croak);
use File::Spec;
use Logger;
use FileWatcher;
use Dirs;
use OSUtils;
use lib Dirs::external_root();
use Term::ANSIColor qw( colored );
use Term::ANSIColor qw(:constants);

my $commandsCounter = 0;
my $commandStartTime;

if (OSUtils::isWindows())
{
    eval('use Win32;');
    eval('use Win32::Process;');
}

sub new
{
    my $instance = {};
    return bless($instance, __PACKAGE__);
}

sub execute
{
    my $this = shift;
    _beforeStart(@_);
    my $exitCode = _system(@_);
    _afterComplete($exitCode);
    return $exitCode;
}

sub executeAndWatchFiles
{
    my ($this, %params) = @_;
    my @command = @{ $params{command} };

    _beforeStart(@command);

    my $fileWatcher = new FileWatcher();
    if (not defined $params{filesToWatch})
    {
        $params{filesToWatch} = [];
    }
    my @filesWatch = @{ $params{filesToWatch} };
    foreach my $ff (@filesWatch)
    {
        my %p = %{$ff};
        $fileWatcher->watch($p{file}, $p{callback}, $p{callbackArg});
    }

    $fileWatcher->start();
    my $exitCode = _system(@command);
    $fileWatcher->stop();

    _afterComplete($exitCode, @command);
    return $exitCode;
}

sub executeAndGetStdOut
{
    my $this = shift;
    my $std_out = qx (@_);
    return ($?, $std_out);
}

sub executeAndGetAllOutput
{
    my $this = shift;
    my $output = qx (@_ 2>&1);
    return ($?, $output);
}

sub fireAndForget
{
    my $this = shift();
    my %params = @_;
    my $executable = $params{executable};
    my @args = @{ $params{args} };

    if (OSUtils::isWindows())
    {
        my $argsTxt = join(' ', @args);
        my $exeFilename = (File::Spec->splitpath($executable))[2];
        my $childProc;
        my $DETACHED_PROCESS = 0x00000008;
        my $res = Win32::Process::Create($childProc, $executable, "$exeFilename $argsTxt", 0, $DETACHED_PROCESS, ".");
        if ($res == 0)
        {
            my $error = Win32::FormatMessage(Win32::GetLastError());
            croak("Can not start process: $error");
        }
        return;
    }
    my $pid = fork();
    if (not defined($pid))
    {
        croak('failed to fork submit process');
    }
    if ($pid == 0)
    {
        exec($executable, @args);
    }
}

# See here on quoting arguments
# - https://blogs.msdn.microsoft.com/twistylittlepassagesallalike/2011/04/23/everyone-quotes-command-line-arguments-the-wrong-way/
# - http://wiki.bash-hackers.org/syntax/quoting
sub _quoteArgumentSwitch
{
    my ($argument) = @_;
    my $isWindows = OSUtils::isWindows();

    # Only quote commandline switches with - or -- and = some value that is not alpha numeric and is not prequoted
    if ($argument =~ /^(-(-)?\w+=)[^"](.*)$/ && $3 !~ /^\w*$/)
    {
        my $result = "\"";
        my $backslashCount = 0;

        foreach (split //, $argument)
        {
            my $char = $_;
            if ($char eq "\\")
            {
                $backslashCount++;
            }
            elsif ($char eq "\"" || (!$isWindows && $char eq "\$"))
            {
                $result = $result . ("\\" x ($backslashCount * 2 + 1));
                $result = $result . $char;
                $backslashCount = 0;
            }
            elsif ($backslashCount > 0)
            {
                $result = $result . ("\\" x $backslashCount);
                $result = $result . $char;
                $backslashCount = 0;
            }
            else
            {
                $result = $result . $char;
            }
        }

        if ($backslashCount > 0)
        {
            $result = $result . ("\\" x ($backslashCount * 2));
        }
        $result = $result . "\"";

        return $result;
    }

    return $argument;
}

sub _getCommand
{
    my @arguments;

    foreach my $arg (@_)
    {
        push(@arguments, _quoteArgumentSwitch($arg));
    }

    return join(' ', @arguments);
}

sub _system
{
    my $exitCode = 0;
    my @arguments = @_;

    my $cmdText = _getCommand(@arguments);
    if (OSUtils::isWindows())
    {
        $exitCode = _systemWin32($cmdText);
    }
    else
    {
        $exitCode = system($cmdText);
        $exitCode >>= 8;
    }
    return $exitCode;
}

sub _systemWin32
{
    my ($cmdText) = @_;
    my $childProc;
    my $NORMAL_PRIORITY_CLASS = 0x00000020;
    my $res = Win32::Process::Create($childProc, $ENV{COMSPEC}, "/c $cmdText", 0, $NORMAL_PRIORITY_CLASS, ".");

    if ($res == 0)
    {
        my $error = Win32::FormatMessage(Win32::GetLastError());
        croak("Can not start process: $error");
    }
    my $exitCode;
    $childProc->Wait(-1);
    $childProc->GetExitCode($exitCode);
    return $exitCode;
}

sub _beforeStart
{
    my (@command) = @_;
    $commandStartTime = time();
    ++$commandsCounter;

    my $cmdText = _getCommand(@command);
    my $commandNumberText = _getColoredCommandNumber($commandsCounter);
    Logger::minimal("\n------Executing command $commandNumberText------");
    Logger::minimal("$cmdText\n");
}

sub _afterComplete
{
    my ($exitCode, @command) = @_;
    my $commandNumberText = _getColoredCommandNumber($commandsCounter);
    my $duration = time() - $commandStartTime;
    Logger::minimal("Command $commandNumberText exited with code $exitCode. Duration: $duration seconds.");
}

sub _getColoredCommandNumber
{
    my ($commandNumber) = @_;
    my $commandNumberText = colored("\#$commandNumber", 'bright_white');
    return $commandNumberText;
}

1;
