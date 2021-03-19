package Tundra;

use strict;
use warnings;
use PrepareWorkingCopy qw (PrepareExternalDependency);
require Exporter;
our @ISA = qw (Exporter);
our @EXPORT = ();
our @EXPORT_OK = qw (call_tundra);

sub call_tundra
{
    my ($schrootprefix, $tundraDagFile, $frontEndCommand, $tundraArgs) = @_;

    PrepareExternalDependency("External/tundra");

    $ENV{"TUNDRA_DAGTOOL_FULLCOMMANDLINE"} = $frontEndCommand;

    my $winflag = "";
    $winflag = "-U " if ($^O eq "MSWin32");

    #check if we're an tty
    if (-t STDOUT && (!defined $ENV{"DOWNSTREAM_STDOUT_CONSUMER_SUPPORTS_COLOR"} || $ENV{"DOWNSTREAM_STDOUT_CONSUMER_SUPPORTS_COLOR"} eq ''))
    {
        $ENV{"DOWNSTREAM_STDOUT_CONSUMER_SUPPORTS_COLOR"} = '1';
    }

    my $finalcmd = $schrootprefix . "External/tundra/builds/$^O/tundra2 $winflag -R $tundraDagFile $tundraArgs";

    system("$finalcmd") eq 0 or die("executing tundra failed with TUNDRA_DAGTOOL_FULLCOMMANDLINE=\"$frontEndCommand\" $finalcmd");
}

1;
