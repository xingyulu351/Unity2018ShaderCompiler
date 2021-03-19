package Process;

use warnings;
use strict;

use IPC::Open3;
use File::Spec;
use IO::File;

sub new
{
    my ($package, $outputConsumer) = @_;
    my $object = {
        started => 0,
        exitCode => -1,
        outputConsumer => $outputConsumer
    };
    return bless($object, $package);
}

sub start
{
    my ($this, @args) = @_;
    my ($in, $out) = ("", "");
    my $pid = open3($in, $out, $out, @args);
    while (<$out>)
    {
        if (defined($this->{outputConsumer}))
        {
            $this->{outputConsumer}($_);
        }
    }
    waitpid($pid, 0);
    close($out);
    $this->{exitCode} = $? >> 8;
    $this->{started} = 1;
}

sub started
{
    my ($this) = @_;
    return $this->{started};
}

sub exitCode
{
    my ($this) = @_;
    return $this->{exitCode};
}

1;
