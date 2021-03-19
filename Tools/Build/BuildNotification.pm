package BuildNotification;

use warnings;
use strict;
use Cwd qw();
use File::Basename qw(basename dirname);
require Exporter;
my $top = File::Spec->rel2abs(dirname(__FILE__) . '/../..');

our @ISA = qw (Exporter);
our @EXPORT = ();
our @EXPORT_OK = qw (InstallNotificationSignalHandler NotifyAndPrintSuccess NotifySuccess);

sub InstallNotificationSignalHandler
{
    if (ShouldNotify())
    {
        $SIG{__DIE__} = sub
        {
            my ($arg) = @_;
            Notify($arg, -1);
            $SIG{__DIE__} = 'DEFAULT';
            die($arg);
        };
    }
}

sub NotifyAndPrintSuccess
{
    (my $message) = @_;
    NotifySuccess($message);
    return print(@_);
}

sub NotifySuccess
{
    (my $message) = @_;
    Notify($message, 1);
}

sub Notify
{
    (my $message, my $success) = @_;
    if (ShouldNotify())
    {
        my $title = $success < 0 ? "Build Failed" : ($success ? "Build Succeeded" : "Build Finished");
        chomp($message);
        my $footer = GetNotificationFooter();
        $message = "$message\n[$footer]";
        if ($^O eq 'MSWin32')
        {
            NotifyWin($message, $success, $title);
        }
        elsif ($^O eq 'darwin')
        {
            NotifyMac($message, $success, $title);
        }
        elsif ($^O eq 'linux')
        {
            NotifyLinux($message, $success, $title);
        }
        else
        {
            print("Unknown platform $^O!\n");
        }
    }
}

sub GetNotificationFooter
{
    my $path = Cwd::cwd();
    my $dir = basename($path);
    my $footer = "$dir";
    my $branchName = `hg branch 2>&1`;
    if ($? == 0)
    {
        chomp($branchName);
        $footer = "$footer - $branchName";
    }
    return $footer;
}

sub ShouldNotify
{
    return $ENV{"ENABLE_UNITY_BUILD_NOTIFICATIONS"};
}

sub NotifyMac
{
    (my $message, my $success, my $title) = @_;
    my @args = ("-title", $title, "-message", $message);
    system("terminal-notifier", @args);
    if ($? != 0)
    {
        print "Notification was requested but terminal-notifier could not be called. Have you installed it with 'sudo gem install terminal-notifier'?\n";
    }
}

sub NotifyWin
{
    (my $message, my $success, my $title) = @_;
    my @args = ("-ExecutionPolicy ByPass -File Tools\\Build\\WinNotify.ps1 -title \"$title\" -message \"$message\"");
    if ($success)
    {
        push(@args, "-success");
    }
    system("powershell", @args);
    if ($? != 0)
    {
        print "Notification was requested but notification script or powershell failed?\n";
    }
}

sub NotifyLinux
{
    (my $message, my $success, my $title) = @_;
    if (-x '/usr/bin/notify-send')
    {
        system('notify-send', '-i', "$top/Editor/Resources/Common/UnityPlayerIcon.png", '-c', 'unity-build', $title, $message);
    }
}

1;
