#!/bin/usr/perl
use strict;
use File::Path qw(mkpath rmtree);
use File::Basename;
use File::Spec;
use FindBin qw'$Bin';
use lib "$FindBin::Bin";
use lib "$FindBin::Bin/../../External/Perl/lib";
use Util qw(IsProcessRunningOnWindows);

chdir(File::Spec->rel2abs("$FindBin::Bin/../.."));

sub ExecWaitProcess
{
    my $name = shift;
    print("Running $name\n");
    system($name) and die("Failed running $name\n");
    $name =~ /\\([^\\]*)\.exe/;
    my $process = $1;
    print("Waiting for $process to finish...\n");
    while (IsProcessRunningOnWindows($process))
    {
        sleep(1);
    }
}

if ($^O eq "MSWin32" or $^O eq "MSWin64")
{
    print("Killing any running Unity sessions...\n");
    system("taskkill /f /t /im Unity.exe >nul 2>&1");

    ExecWaitProcess("build\\Windows64EditorInstaller\\UnitySetup64.exe /S /D=C:\\Program Files\\Unity");

    if (-d 'build\\WindowsDocumentationInstaller')
    {
        ExecWaitProcess("build\\WindowsDocumentationInstaller\\UnityDocumentationSetup.exe /S");
    }
    else
    {
        print("Documentation installer is not available\n");
    }

    if (-d 'build\\TargetSupportInstaller')
    {
        my @installers = glob("build\\TargetSupportInstaller\\*.exe");
        foreach my $inst (@installers)
        {
            ExecWaitProcess("$inst /S");
        }
    }
    else
    {
        print("TargetSupportInstaller installers are not available\n");
    }
}
else
{
    print("Killing any running Unity sessions...\n");
    system("killall Unity");

    do
    {
        if (-d "/Applications/Unity")
        {
            print("Removing existing installed version of Unity...\n");
            rmtree("/Applications/Unity") or die("Failed to remove Unity");
        }

        my $installerResult;
        if ($ENV{"UNITY_THISISABUILDMACHINE"} == "1")
        {
            print("Running editor installer...\n");
            $installerResult = sudo("installer -dumplog -pkg build/MacEditorInstaller/Unity.pkg -target /", $ENV{"INTEGRATION_TESTS_INSTALL_PASSWORD"});

            if (-d 'build/MacDocumentationInstaller')
            {
                print("Running Documentation installer...\n");
                $installerResult =
                    sudo("installer -dumplog -pkg build/MacDocumentationInstaller/Documentation.pkg -target /", $ENV{"INTEGRATION_TESTS_INSTALL_PASSWORD"});
            }
            else
            {
                print("Documentation installer is not available\n");
            }

            if (-d 'build/MacEditorTargetInstaller')
            {
                my @installers = glob("build/MacEditorTargetInstaller/*.pkg");
                foreach my $inst (@installers)
                {
                    print("Running installer: $inst\n");
                    $installerResult = sudo("installer -dumplog -pkg $inst -target /", $ENV{"INTEGRATION_TESTS_INSTALL_PASSWORD"});
                }
            }
            else
            {
                print("MacEditorTargetInstaller installers are not available\n");
            }
        }
        else
        {
            print("Running editor installer with sudo...\n");
            $installerResult = runAndGetResult("sudo installer -dumplog -pkg build/MacEditorInstaller/Unity.pkg -target /");
        }

        # Evaluate installation success
        my $result = "failure";
        while (<$installerResult>)
        {
            $result = $_;
            print("$_\n");
        }
        if (!($result =~ m/successful.$/))
        {
            die("Failure");
        }
        if (-d "/Applications/Unity/Unity.localized")
        {
            print("Install went into /Applications/Unity/Unity.localized . Try to find out why this happens! Retrying.\n");
        }
    } while (-d "/Applications/Unity/Unity.localized");

    print("Success\n");
}

sub copyFromDiskImage
{
    my $imagePath = shift;
    my $mountPath = shift;
    my $copyFromMount = shift;
    my $copyToPath = shift;

    system("echo Copying from $imagePath...");
    system("hdiutil eject \"$mountPath\" -quiet");
    system("hdid \"$imagePath\" -quiet");
    system("cp -R \"$mountPath/$copyFromMount\" \"$copyToPath\"");
    system("hdiutil eject \"$mountPath\" -quiet");
}

sub sudo
{
    my $command = shift;
    my $password = shift;

    my $command = "sudo -S -p \"\" $command";
    my ($processStdin, $processStdout, $processStderr);

    use IPC::Open3;
    my $process = open3($processStdin, $processStdout, $processStderr, $command);
    sleep 1;
    print $processStdin "$password\n";

    waitpid($process, 0);

    return $processStdout;
}

sub runAndGetResult
{
    my $command = shift;

    my $processStdout;

    open($processStdout, "$command|") or die("Failed to run command $command");

    return $processStdout;
}
