package ParamUtils;

use warnings;
use strict;

use Carp qw (croak);
use Exporter 'import';
our @EXPORT_OK = qw (verifyParamExistsOrCroak);

sub verifyParamExistsOrCroak
{
    my ($param, %args) = @_;
    if (not $args{$param})
    {
        croak("$param must be defined");
    }
}

1;
