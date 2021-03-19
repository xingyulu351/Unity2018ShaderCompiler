package Environments::Win;

use parent Environment;

use warnings;
use strict;
use BuildEngines::MSBuild;

use NUnitCommandLine;

sub new
{
    my ($pkg) = @_;
    my $instance = $pkg->SUPER::new();
    $instance->{buildEngine} = new BuildEngines::MSBuild();
    return bless($instance, $pkg);
}

sub getName
{
    my ($this) = @_;
    return 'win';
}

sub runDotNetProgram
{
    my ($this, %args) = @_;
    my $program = $args{program};
    my @programArgs;
    if (defined($args{programArgs}))
    {
        push(@programArgs, @{ $args{programArgs} });
    }
    return $this->{systemCall}->executeAndWatchFiles(
        command => [$program, @programArgs],
        filesToWatch => $args{filesToWatch}
    );
}

sub runNUnitProject
{
    my ($this, %args) = @_;
    my @dlls = @{ $args{dlls} };
    my @nunitcommandLine = NUnitCommandLine::build(%args);
    return $this->{systemCall}->executeAndWatchFiles(
        command => \@nunitcommandLine,
        filesToWatch => $args{filesToWatch}
    );
}

1;
