package MercurialRevision;

use warnings;
use strict;

use Carp qw(croak);

sub new
{
    my ($pkg, $systemCall) = @_;
    my %params = @_;
    my $instance = {};
    bless($instance, $pkg);

    my $output = _runHgCommand($systemCall, 'log', '--rev', '.', '--limit=1', '--template', '"{node|short} {rev} {date|hgdate} {branch}\n"');

    ($instance->{id}, $instance->{revNumber}, $instance->{date}, $instance->{branch}) = $output =~ m/([0-9a-f]+) (\d+) (\d+) -?\d+ (.*)/i;

    return $instance;
}

sub getId
{
    my ($this) = @_;
    return $this->{id};
}

sub getVcsType
{
    return 'mercurial';
}

sub getDate
{
    my ($this) = @_;
    return $this->{date};
}

sub getRevNumber
{
    my ($this) = @_;
    return $this->{revNumber};
}

sub getBranch
{
    my ($this) = @_;
    return $this->{branch};
}

sub getBranches
{
    my ($this) = @_;
    my @res;
    return \@res;
}

sub _runHgCommand
{
    my ($systemCall, @command) = @_;
    unshift(@command, 'hg');
    my ($exitCode, $output) = $systemCall->executeAndGetAllOutput(@command);
    my $success = 1;
    if ($exitCode != 0)
    {
        my $command = join(' ', @command);
        my $message = "Failed to execute '$command'\nCommand output:\n$output\nExit code: $exitCode\n";
        croak($message);
    }
    return $output;
}

1;
