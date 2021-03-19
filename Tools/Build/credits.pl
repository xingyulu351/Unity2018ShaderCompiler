#!/usr/bin/perl
use strict;
use warnings;

## Simple tool filter extracted credits

my $create_final = 0;
my $filename = '';

if (@ARGV > 0)
{
    $filename = $ARGV[0];
    if ($ARGV[0] eq 'final')
    {
        $create_final = 1;
        $filename = $ARGV[1];
    }
}
else
{
    print "Command line usage:\n\n\tcredits.pl [final] <filename>\n";
    exit(1);
}

open my $fh, '<', $filename or die "Cannot open $filename: $!";

while (my $line = <$fh>)
{
    chomp $line;
    if ($line ne "")
    {
        # If it's the header line, skip
        next if ($line =~ /Tracking,Employee/);

        my @items = split(/,/, $line);

        # remove the first column, as it's status tracking
        my $status = shift @items;

        if ($create_final == 1)
        {
            next if length($status) > 0;

            # Drop employee id
            shift @items;

            # Drop specific office
            if (@items > 2)
            {
                splice(@items, 2, 1);
            }
        }
        else
        {
            # Remove alumni column for diff
            splice(@items, 2, 1);
        }

        pop @items;    # notes
        pop @items;    # gravatar hash
        pop @items;    # email
        pop @items;    # nationality

        print join(',', @items) . "\n";
    }
}
close($fh);
