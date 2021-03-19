package Builder;

use warnings;
use strict;

use SystemCall;

use Carp qw (croak);

sub new
{
    my ($pkg) = @_;
    my $instance = {
        systemCall => new SystemCall(),
        config => 'Debug'
    };

    return bless($instance, $pkg);
}

sub name
{
    croak("Method 'name' must be implemented in child classes");
}

sub build
{
    croak("Method 'build' must be implemented in child classes");
}

sub setSystemCall
{
    my ($this, $systemCall) = @_;
    $this->{systemCall} = $systemCall;
}

sub getSystemCall
{
    my ($this) = @_;
    return $this->{systemCall};
}

sub setRebuild
{
    my ($this, $rebuild) = @_;
    $this->{rebuild} = $rebuild;
}

sub getRebuild
{
    my ($this) = @_;
    return $this->{rebuild};
}

sub setConfig
{
    my ($this, $config) = @_;
    $this->{config} = $config;
}

sub getConfig
{
    my ($this) = @_;
    return $this->{config};
}

sub callSystemAndGetStdOut
{
    my ($this, @params) = @_;
    my $systemCall = $this->{systemCall};
    return $systemCall->executeAndGetStdOut(@params);
}

sub executeAndWatchFiles
{
    my ($this, %params) = @_;
    return $this->{systemCall}->executeAndWatchFiles(%params);
}

sub dumpBuildLogAndCroak
{
    my ($this, $project, $logfile) = @_;
    my $buildLogText = join("\n", FileUtils::readAllLines($logfile));
    Logger::error($buildLogText);
    croak("Compilation of $project failed.");
}

1;
