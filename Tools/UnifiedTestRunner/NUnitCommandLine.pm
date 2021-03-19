package NUnitCommandLine;

use warnings;
use strict;

use Dirs;
use File::Spec::Functions qw (canonpath);

sub build
{
    my (%args) = @_;
    my $root = Dirs::getRoot();
    my $nunit = canonpath("$root/External/NUnit/nunit-console.exe");

    my @result;
    push(@result, $nunit, '--encoding=utf-8', @{ $args{dlls} });

    my $directArgs = $args{directRunnerArgs};
    if (defined($directArgs))
    {
        push(@result, @$directArgs);
    }

    return @result;
}

1;
