package FileWatcher;

use LineTail;
use File::Spec::Functions qw (canonpath);
use Dirs;
use lib Dirs::external_root();
use List::MoreUtils qw (any);
use warnings;
use strict;

sub new
{
    my $instance = {};
    return bless($instance, __PACKAGE__);
}

sub watch
{
    my ($this, $fileName, $receiver, $arg) = @_;
    if (not exists($this->{watchedFiles}))
    {
        $this->{watchedFiles} = [];
    }

    $fileName = canonpath($fileName);
    if (not any { $_ eq $fileName } @{ $this->{watchedFiles} })
    {
        push(@{ $this->{watchedFiles} }, $fileName);
    }

    if (not exists($this->{$fileName}))
    {
        $this->{$fileName} = [];
    }
    push(@{ $this->{$fileName} }, $receiver);

    $this->{ $fileName . "arg" } = $arg;
}

sub start
{
    my ($this) = @_;
    foreach my $fileName (@{ $this->{watchedFiles} })
    {
        my $agregatedReceiver = sub
        {
            foreach my $receiver (@{ $this->{$fileName} })
            {
                &{$receiver}(@_);
            }
        };
        my $arg = $this->{ $fileName . 'arg' };
        $this->{ $fileName . 'tail' } = new LineTail(file => $fileName, receiver => $agregatedReceiver, arg => $arg);
        $this->{ $fileName . 'tail' }->start();
    }
}

sub stop
{
    my ($this) = @_;
    foreach my $fileName (@{ $this->{watchedFiles} })
    {
        $this->{ $fileName . 'tail' }->stop();
    }
}

1;
