package ProgressPresenters::Simple;

use warnings;
use strict;

sub new
{
    my $instance = {};
    return bless($instance, __PACKAGE__);
}

sub getName
{
    return "simple";
}

sub stepStarted
{
    my ($this, %args) = @_;
    my $message = $this->_buildMessage('Started', %args);
    Logger::minimal(msg => $message, resetLine => $args{resetLine});
}

sub stepDone
{
    my ($this, %args) = @_;
    my $message = $this->_buildMessage('Completed', %args);
    Logger::minimal(msg => $message, resetLine => $args{resetLine});
}

sub stepError
{
    my ($this, %args) = @_;
    my $message = $this->_buildMessage('Failed', %args);
    Logger::error(msg => $message, resetLine => $args{resetLine});
}

sub _buildMessage
{
    my ($this, $prefix, %args) = @_;
    my $message = "";
    if (defined($prefix))
    {
        $message .= "$prefix:";
    }
    $message = _writeDescription($message, %args);
    $message = _writeProgress($message, %args);
    $message = _writeError($message, %args);
    $message = _writeProgressDetails($message, %args);
    $message = _writeDuration($message, %args);
    return $message;
}

sub _writeDescription
{
    my ($message, %args) = @_;
    if (not $args{description})
    {
        return $message;
    }
    $message .= " '$args{description}'";
    return $message;
}

sub _writeProgress
{
    my ($message, %args) = @_;
    my $progress = $args{progress};
    if (not defined($progress))
    {
        return $message;
    }

    my $total = $progress->{total};
    if (not defined($total))
    {
        $total = '?';
    }

    return _append($message, "[$progress->{current}/$total]");
}

sub _writeError
{
    my ($message, %args) = @_;
    my $error = $args{error};
    if ($error)
    {
        return _append($message, "'$error'");
    }
    return $message;
}

sub _writeProgressDetails
{
    my ($message, %args) = @_;
    return _append($message, $args{progressDetails});
}

sub _writeDuration
{
    my ($message, %args) = @_;
    my $duration = $args{duration};
    if (defined($duration))
    {
        my $durationInSec = $duration / 1000.0;
        return _append($message, "$durationInSec seconds");
    }
    return $message;
}

sub _append
{
    my ($message, $suffix) = @_;
    if (not defined($suffix))
    {
        return $message;
    }
    if (defined($message) and length($message) > 0)
    {
        $message .= " $suffix";
    }
    else
    {
        $message = $suffix;
    }
    return $message;
}

1;
