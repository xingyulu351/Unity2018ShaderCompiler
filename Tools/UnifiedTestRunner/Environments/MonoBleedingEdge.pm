package Environments::MonoBleedingEdge;

use parent Environment;

use warnings;
use strict;

use File::Spec::Functions qw (canonpath);
use Dirs;
use NUnitCommandLine;
use File::Basename;
use FileUtils qw (getDirName);
use SystemCall;

my $root = Dirs::getRoot();
my $monoPrefix = canonpath("$root/External/MonoBleedingEdge/builds/monodistribution");
my $mono = canonpath("$monoPrefix/bin/mono");
my $path_separator = ':';

sub new
{
    my ($pkg, $runContext) = @_;
    my $instance = $pkg->SUPER::new($runContext);
    return bless($instance, $pkg);
}

sub getName
{
    return 'monobe';
}

sub runDotNetProgram
{
    my ($this, %args) = @_;

    my $program = $args{program};
    my @runArgs;
    if ($args{programArgs})
    {
        push(@runArgs, @{ $args{programArgs} });
    }
    my @monoArgs = _getMonoArgs(%args);
    return $this->{systemCall}->executeAndWatchFiles(
        command => [$mono, @monoArgs, $program, @runArgs],
        filesToWatch => $args{filesToWatch}
    );
}

sub runNUnitProject
{
    my ($this, %args) = @_;
    my @monoArgs = _getMonoArgs(%args);
    my @nunitArgs = _getNUnitArgs(%args);
    $args{directRunnerArgs} = \@nunitArgs;
    my @nunitCommandLine = NUnitCommandLine::build(%args);
    return $this->{systemCall}->executeAndWatchFiles(
        command => [$mono, @monoArgs, @nunitCommandLine],
        filesToWatch => $args{filesToWatch}
    );
}

sub _getMonoArgs
{
    my (%args) = @_;
    my @monoArgs;
    if ($args{monoArgs})
    {
        @monoArgs = @{ $args{monoArgs} };
    }
    return @monoArgs;
}

sub _getNUnitArgs
{
    my (%args) = @_;
    my @directRunnerArgs = ('-noshadow', '-domain=None');
    if (defined($args{directRunnerArgs}))
    {
        @directRunnerArgs = @{ $args{directRunnerArgs} };
    }
    return @directRunnerArgs;
}

1;
