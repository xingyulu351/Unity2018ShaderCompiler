package JamWrapper;

use warnings;
use strict;
use File::Spec::Functions qw (canonpath);
use Dirs;
use SystemCall;
use Carp qw (croak);
use Logger;

require Exporter;

our @ISA = qw (Exporter);
our @EXPORT_OK = qw (Jam);

my $root = Dirs::getRoot();

sub Jam
{
    my ($target, $logFile) = @_;
    my $systemCall = new SystemCall();
    my $jam = canonpath("$root/jam.pl");
    my @args = ('perl', $jam, '-a', '-q', '-sUNITY_RUN_NATIVE_TESTS_DURING_BUILD=0', $target);
    Logger::minimal("Running $jam " . join(" ", @args));
    if (defined($logFile))
    {
        push(@args, ">$logFile");
    }
    my $exitCode = $systemCall->execute(@args);
    if ($exitCode != 0)
    {
        croak('Failed to execute jam command: ' . join(' ', @args));
    }
}

1;
