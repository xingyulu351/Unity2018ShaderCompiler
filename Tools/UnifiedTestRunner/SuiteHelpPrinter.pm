package SuiteHelpPrinter;

use warnings;
use strict;

use Option;

use Carp qw (croak);
use OptionsHelpPrinter;
use ExamplesHelpPrinter;
use Commands;

sub getText
{
    my %args = @_;
    if (not defined($args{name}))
    {
        croak("suite name must be defined");
    }
    my @result;
    _writeLine(\@result, $args{description});
    _writePair(\@result, 'Prerequisites', $args{prerequisites});
    _writePair(\@result, 'Details', $args{url});
    _writeExamples(\@result, @_);
    _writeLine(\@result, OptionsHelpPrinter::getText(@{ $args{options} }));
    return join("\n", @result);
}

sub _writeExamples
{
    my ($array, %args) = @_;
    my $examples = $args{examples};

    if (not defined($examples))
    {
        return;
    }

    _writeLine($array, 'Examples');

    my $utrPredix = Commands::getUtrPrefix();
    my $text = ExamplesHelpPrinter::getText(
        commandPrefix => "$utrPredix --suite=$args{name}",
        examples => $args{examples}
    );

    _writeLine($array, $text);
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
