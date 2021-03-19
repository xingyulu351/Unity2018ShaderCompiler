package UnityTestProtocol::FilterExpressionBuilder;

use warnings;
use strict;

sub convert
{
    my ($expression, $var) = @_;
    $expression =~ s/\$/\$$var/gi;
    $expression =~ s/[.]/\-\>/g;
    $expression =~ s/->([a-z0-9]+)/->{$1}/gi;
    $expression =~ s/==/ eq /gi;
    return $expression;
}

1;
