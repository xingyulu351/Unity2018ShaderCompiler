#!/usr/bin/perl

# Each C# test runner e.g. AllInOneRunner can expose its options in
# json format. UTR needs those options to avoid duplication and to pass-thought
# them to target runner(if no special processing is required)
# This script will go through all the runner plugins
# and update JSON file definitions for them. If a runner does not
# support it - it will be the no-op.
# All definitions are stored in Tools/UnifiedTestRunner/Configs

use warnings;
use strict;

use FindBin qw ($Bin);
use lib $Bin;
use Dirs;
use File::Spec::Functions qw (catfile);
use SuiteRunners;

my @runners = SuiteRunners::getRunners();
my $testPlPath = catfile(Dirs::UTR_root(), 'test.pl');

foreach my $r (@runners)
{
    my @cmd = ('perl', $testPlPath, '--suite=' . $r->getName(), '--update-runner-options');
    if ($r->getName() eq 'collab')
    {
        next;
    }

    my $exitCode = _runCommand(@cmd);
    if ($exitCode != 0)
    {
        die("Failed to update help for " . $r->getName());
    }
}

my $exitCode = _runCommand('perl', 'updateAutomationDocs.pl');
if ($exitCode != 0)
{
    die("Failed to automation docs");
}

sub _runCommand
{
    my (@cmd) = @_;
    print("Running: " . join(' ', @cmd) . "\n");
    return system(@cmd);
}

exit(0);
