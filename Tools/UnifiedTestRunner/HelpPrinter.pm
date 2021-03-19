package HelpPrinter;

use warnings;
use strict;
use Carp qw (croak);
use OptionsHelpPrinter;
use ExamplesHelpPrinter;

sub getText
{
    if (not @_)
    {
        croak("Invalid parameters");
    }

    my %args = @_;
    my @result;
    _writeLine(\@result, $args{description});
    _writePair(\@result, "\nDetails", $args{url});
    _writePair(\@result, "\nPrerequisites", $args{prerequisites});
    _writeLine(\@result, "\nUsage examples:");
    _writeLine(\@result, ExamplesHelpPrinter::getText(@_));
    _writeLine(\@result, OptionsHelpPrinter::getText(@{ $args{options} }));
    return join("\n", @result);
}

sub _writePair
{
    my ($array, $key, $value) = @_;
    if (not defined($value))
    {
        return;
    }
    _writeLine($array, "$key: $value");
}

sub _writeLine
{
    my ($array, $line) = @_;
    if ($line)
    {
        push(@$array, $line);
    }
}

1;
