package ExamplesHelpPrinter;

use warnings;
use strict;

sub getText
{
    if (not @_)
    {
        return '';
    }

    my %args = @_;
    my $examples = $args{examples};
    if (not defined($examples))
    {
        return '';
    }

    my @result;
    my $commandPrefix = $args{commandPrefix};
    foreach my $x (@{$examples})
    {
        push(@result, "    $x->{description}");
        my $command = $x->{command};
        if (defined($commandPrefix) and defined($command))
        {
            $command = "$commandPrefix $command";
        }
        if (defined($command))
        {
            push(@result, "        $command");
        }
    }

    return join("\n", @result) . "\n";
}

1;
