package Printers::Console;

use warnings;
use strict;

use Logger;

use IPC::Cmd qw(can_run);

sub new
{
    return bless({}, __PACKAGE__);
}

sub resetLine
{
    if (_isStdOutRedirected())
    {
        return;
    }
    my $SPACE = q{ };
    my $width = _getConsoleCharWidth();
    my $CLEAN_LINE = ($SPACE x $width) . ("\b" x $width);
    Logger::raw("\r" . $CLEAN_LINE);
}

sub printFormatted
{
    my ($this, $text) = @_;
    my $textToPrint = _prepareToPrint($text);
    Logger::raw($textToPrint);
}

sub printRaw
{
    my ($text) = @_;
    Logger::raw($text);
}

sub _getConsoleCharWidth
{
    my ($wchar) = GetTerminalSize();
    return $wchar - 1;
}

sub _prepareToPrint
{
    my ($text) = @_;
    if (_isStdOutRedirected())
    {
        return "$text\n";
    }

    my $width = _getConsoleCharWidth();
    $text = Logger::formatMessage($text);
    if (length($text) >= $width)
    {
        return substr($text, 0, $width);
    }

    return $text;
}

sub _isStdOutRedirected
{
    ## no critic (ProhibitInteractiveTes)
    return not(-t STDOUT);
}
1;
