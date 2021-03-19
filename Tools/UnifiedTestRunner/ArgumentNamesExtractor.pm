package ArgumentNamesExtractor;

use warnings;
use strict;

sub extract
{
    my @result;
    foreach my $a (@_)
    {
        my $argName = $a;
        $argName =~ s/^[-]{1,2}//;
        if ($argName =~ m/=/)
        {
            ($argName) = split(m/=/, $argName);
        }
        push(@result, $argName);
    }
    return @result;
}

1;
