package GUIDUtils;
use warnings;
use strict;

sub extractGUID
{
    my ($line) = @_;
    my ($guid) = $line =~ m/([A-Fa-f0-9]{8}[\-][A-Fa-f0-9]{4}[\-][A-Fa-f0-9]{4}[\-][A-Fa-f0-9]{4}[\-]([A-Fa-f0-9]){12})/gi;
    if ($guid)
    {
        return uc($guid);
    }
    return;
}

1;
