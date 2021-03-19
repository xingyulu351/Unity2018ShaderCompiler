package Util;
use warnings;
use strict;

require Exporter;
our @ISA = qw(Exporter);
our @EXPORT_OK = qw(IsProcessRunningOnWindows);

sub IsProcessRunningOnWindows
{
    my $app = shift;
    $app .= '.exe';
    my $res = `tasklist /FI "IMAGENAME eq $app" 2>&1`;
    return ($res =~ /$app/);
}

1;
